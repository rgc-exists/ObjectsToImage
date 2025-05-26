#include <Geode/Geode.hpp>
#include <iostream>

#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;


void drawRecursive(CCNode* parent) {

	CCSprite* sprite = static_cast<CCSprite*>(parent);
	ccBlendFunc ogBlendFunc = sprite->getBlendFunc();

	CCSpriteBatchNode* batchNode = sprite->getBatchNode();
	if (batchNode != nullptr) {
		ccBlendFunc batchNodeBlendFunc = batchNode->getBlendFunc();
		sprite->setBlendFunc(batchNodeBlendFunc);
	}

	GameObject* gameObject = dynamic_cast<GameObject*>(parent);
	if (gameObject != nullptr) {
		log::debug("{}", gameObject->m_baseColor->m_customColor);
		gameObject->setOpacity(gameObject->m_baseColor->m_opacity * 255);
	}

	sprite->draw();
	sprite->setBlendFunc(ogBlendFunc);

	CCArray* children = parent->getChildren();
	for (int i = 0; i < parent->getChildrenCount(); i++) {
		CCNode* childNode = (CCNode*)children->objectAtIndex(i);
		drawRecursive(childNode);
	}
}


class $modify(EditorUIHook, EditorUI) {

	struct Fields {
		bool isTakingImage = false;
		CCArray* tempSelected = nullptr;
	};

	void onButton(CCObject*) {

		log::debug("onButton called.");

		CCArray* selected = getSelectedObjects();
		log::debug("selected size: {}", selected->count());

		m_fields->tempSelected = selected->shallowCopy();
		m_fields->tempSelected->retain();
		deselectAll();
		log::debug("m_fields->tempSelected: {}", m_fields->tempSelected);
		log::debug("m_fields->tempSelected size: {}", m_fields->tempSelected->count());
		m_fields->isTakingImage = true;

		std::vector<CCObject*> objs;
		for (int i = 0; i < m_fields->tempSelected->count(); i++)
			objs.push_back(m_fields->tempSelected->objectAtIndex(i));

	}

	void saveObjectsToImage(CCArray* objects, const char* filePath) {
		log::debug("saving objects to {}", filePath);

		log::debug("creating render texture...");
		CCRenderTexture* renderTexture = CCRenderTexture::create(1920, 1080, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
		log::debug("running beginWithClear...");
		renderTexture->beginWithClear(0, 0, 0, 0);


		log::debug("beginWithClear done.");
		for (int i = 0; i < objects->count(); i++) {
			CCNode* node = static_cast<CCNode*>(objects->objectAtIndex(i));
			GameObject* gameObj = static_cast<GameObject*>(node);

			ccColor3B selectCol = ccColor3B(0, 255, 0);
			drawRecursive(node);
		}
		CCImage* image = renderTexture->newCCImage();
		log::debug("saving CCImage to file...");
		image->saveToFile(filePath, false);
		renderTexture->end();
		log::debug("successfully saved objects!");
	}

	void sortByZOrder(CCArray* objects) {
		GameObject** tempSelectedBegin = reinterpret_cast<GameObject**>(objects->data->arr);

		log::debug("About to sort...");
		std::sort(tempSelectedBegin, tempSelectedBegin + objects->data->num, [](GameObject* a, GameObject* b) {

			log::debug("Sorting a to b...");
			bool prioritizedOnHierarchy = false;
			
			if (a->getParent() != nullptr && b->getParent() != nullptr) {
				int a_index = a->getParent()->getChildren()->indexOfObject(a);
				int b_index = b->getParent()->getChildren()->indexOfObject(b);
				log::debug("a index: {}, b index: {}", a_index, b_index);
				prioritizedOnHierarchy = a_index < b_index;

				CCSpriteBatchNode* a_batchNode = a->getBatchNode();
				CCSpriteBatchNode* b_batchNode = b->getBatchNode();

				if (a_batchNode != nullptr && b_batchNode != nullptr) {

					log::debug("batch node exists.");
					int a_batchNode_index = a_batchNode->getParent()->getChildren()->indexOfObject(a_batchNode);
					int b_batchNode_index = b_batchNode->getParent()->getChildren()->indexOfObject(b_batchNode);
					log::debug("batch node a index: {}, batch node b index: {}", a_batchNode_index, b_batchNode_index);

					prioritizedOnHierarchy = (a_batchNode_index > b_batchNode_index) || prioritizedOnHierarchy;
				}
			}
			else {
				log::debug("Either a or b has no parent. Skipping hierarchy logic...");
			}

			log::debug("Returning answer...");
			return a->getZOrder() < b->getZOrder()
				|| (int)a->getObjectZLayer() < (int)b->getObjectZLayer()
				|| a->m_editorLayer < b->m_editorLayer
				|| prioritizedOnHierarchy;
			});

	}

	void customUpdate(float p0) {
		if (m_fields->isTakingImage) {
			LevelEditorLayer::get()->updateObjectColors(m_fields->tempSelected);
			
			log::debug("sorting m_fields->tempSelected by z order...");
			sortByZOrder(m_fields->tempSelected);

			log::debug("taking image...");
			log::debug("m_fields->tempSelected: {}", m_fields->tempSelected);
			saveObjectsToImage(m_fields->tempSelected, "C:\\Users\\rando\\AppData\\Local\\GeometryDash\\test_thing.png");
			log::debug("saved image!");

			selectObjects(m_fields->tempSelected, false);
			m_fields->tempSelected->release();
			log::debug("reselected objects.");
		}
		m_fields->isTakingImage = false;
	}

	void createMoveMenu() {
		log::debug("createMoveMenu called.");
		EditorUI::createMoveMenu();
		log::debug("ORIGINAL createMoveMenu called.");

		CCSprite* sprite = CCSprite::create("dumButton.png"_spr);
		auto btn = CCMenuItemSpriteExtra::create(
			sprite,
			this,
			menu_selector(EditorUIHook::onButton)
		);
		log::debug("btn created.");
		m_editButtonBar->m_buttonArray->addObject(btn);
		log::debug("btn added as object.");

		auto rows = GameManager::sharedState()->getIntGameVariable("0049");
		log::debug("Got game variable 0049.");
		auto cols = GameManager::sharedState()->getIntGameVariable("0050");
		log::debug("Got game variable 0050.");
		m_editButtonBar->reloadItems(rows, cols);
		log::debug("Reloaded editorButtonBar items.");

		this->schedule(schedule_selector(EditorUIHook::customUpdate), 0.05f);

	}

};