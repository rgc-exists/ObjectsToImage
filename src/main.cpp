#include <Geode/Geode.hpp>
#include <iostream>

#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/EditorUI.hpp>

#include <Geode/utils/file.hpp>
#include <Geode/utils/string.hpp>

using namespace geode::prelude;
using namespace geode::utils::file;

std::filesystem::path defaultDirectory;
bool blendingApproximation = true;

$execute {
	defaultDirectory = Mod::get()->getSettingValue<std::filesystem::path>("default-directory");\

	listenForSettingChanges("default-directory", [](std::filesystem::path value) {
		defaultDirectory = value;
	});
};

class $modify(EditorUIHook, EditorUI) {

	struct Fields {
		geode::EventListener<geode::Task<geode::Result<std::filesystem::path>>>
			m_pickListener;
	};


	void saveToChosenPath( 
		Task<Result<std::filesystem::path>>::Event* event) {

		if (event->isCancelled()) {
			return;
		}
		if (auto result = event->getValue()) {
			if (result->isErr()) {
				log::error("ERROR: Result of file::pick was an error.");
				return;
			}

			CCArray* selected = getSelectedObjects()->shallowCopy();
			std::filesystem::path path = result->unwrap();

			defaultDirectory = path.parent_path();
			std::string pathStr = string::pathToString(path);
			if (!pathStr.ends_with(".png"))
				pathStr += ".png";

			saveObjectsToImage(selected, pathStr.c_str());
		}
	}


	void onButton(CCObject * sender) {

		if (getSelectedObjects()->count() <= 0) {
			return;
		}

		geode::createQuickPopup(
			"APPROXIMATE BLENDING?",
			"TL;DR: Try both and see what works.\n\nDue to how colors with blending enabled work, it is impossible to perfectly recreate the effect with a transparent background.\nWould you like to enable the somewhat innacurate fix?",
			"Yes", "No",
			[this](auto, bool btn2) {

				blendingApproximation = !btn2;

				auto options = FilePickOptions();
				auto filter = FilePickOptions::Filter();
				filter.description = "PNG Images"; filter.files = { "*.png" };
				options.defaultPath = defaultDirectory;
				options.filters = { filter };

				m_fields->m_pickListener.bind(this, &EditorUIHook::saveToChosenPath);
				m_fields->m_pickListener.setFilter(file::pick(file::PickMode::SaveFile, options));
			}
		);
	}

	static void saveObjectsToImage(CCArray* objects, const char* filePath) {

		std::ostringstream buffer;
		for (int i = 0; i < objects->count(); i++) {
			GameObject* obj = static_cast<GameObject*>(objects->objectAtIndex(i));
			buffer << obj->getSaveString(LevelEditorLayer::get()) << ";";
		}
		std::string objString = buffer.str();

		CCArray* objArray = CCArray::create();
		CCSprite* sprite = EditorUI::get()->spriteFromObjectString(objString, false, false, INT_MAX, objArray, nullptr, nullptr);
		LevelEditorLayer::get()->updateObjectColors(objArray);

		for (int i = 0; i < objArray->count(); i++) {
			GameObject* gameObject = static_cast<GameObject*>(objArray->objectAtIndex(i));

			gameObject->setOpacity(255);
			if (blendingApproximation) {
				if (gameObject->m_shouldBlendBase) {
					applyFakeBlendingOpacity(static_cast<CCSprite*>(gameObject));
				}
				if (gameObject->m_shouldBlendDetail) {
					applyFakeBlendingOpacity(gameObject->m_colorSprite);
				}
			}
		}


		CCSize scaledContentSize = sprite->getScaledContentSize();
		sprite->setPosition(scaledContentSize / 2);

		CCRenderTexture* renderTexture = CCRenderTexture::create(scaledContentSize.width, scaledContentSize.height, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
		renderTexture->beginWithClear(0, 0, 0, 0);
		sprite->visit();
		CCImage* image = renderTexture->newCCImage();
		image->saveToFile(filePath, false);
		renderTexture->end();

	}

	static void applyFakeBlendingOpacity(CCSprite* sprite) {
		ccColor3B color = sprite->getColor();
		sprite->setOpacity(std::max({ color.r, color.g, color.b }));
	}

	void createMoveMenu() {
		EditorUI::createMoveMenu();

		auto* btn = getSpriteButton("importButton.png"_spr, menu_selector(EditorUIHook::onButton), nullptr, 0.75f);
		m_editButtonBar->m_buttonArray->addObject(btn);

		auto rows = GameManager::sharedState()->getIntGameVariable("0049");
		auto cols = GameManager::sharedState()->getIntGameVariable("0050");
		m_editButtonBar->reloadItems(rows, cols);

	}

};