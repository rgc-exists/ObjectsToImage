#include <Geode/Geode.hpp>
#include <iostream>

#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/EditorUI.hpp>

#include <Geode/utils/file.hpp>
#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;
using namespace geode::utils::file;

std::filesystem::path defaultDirectory;

$execute {
	defaultDirectory = Mod::get()->getSettingValue<std::filesystem::path>("default-directory");

	listenForSettingChanges("default-directory", [](std::filesystem::path value) {
		defaultDirectory = value;
	});
};

class $modify(EditorUIHook, EditorUI) {

	void onButton(CCObject * sender) {
		CCArray* selected = getSelectedObjects()->shallowCopy();

		auto options = FilePickOptions();
		auto filter = FilePickOptions::Filter();
		filter.description = "PNG Images"; filter.files = { "*.png" };
		options.defaultPath = defaultDirectory;
		options.filters = { filter };

		file::pick(PickMode::SaveFile, options).listen(
			[selected](Result<std::filesystem::path>* result) {
				if (!result->isOk()) {
					return;
				}

				std::filesystem::path path = result->unwrap();
				defaultDirectory = path.parent_path();
				if (!path.string().ends_with(".png"))
					path += ".png";

				saveObjectsToImage(selected, path.string().c_str());
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
			gameObject->setOpacity(gameObject->m_baseColor->m_opacity * 255);
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

	void createMoveMenu() {
		EditorUI::createMoveMenu();

		auto* btn = getSpriteButton("importButton.png"_spr, menu_selector(EditorUIHook::onButton), nullptr, 0.75f);
		m_editButtonBar->m_buttonArray->addObject(btn);

		auto rows = GameManager::sharedState()->getIntGameVariable("0049");
		auto cols = GameManager::sharedState()->getIntGameVariable("0050");
		m_editButtonBar->reloadItems(rows, cols);

	}

};