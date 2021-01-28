/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "ui/CocosGUI.h"
#include "MainScene.h"
#include "MainMenuScene.h"
#include "GameResultScene.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;


Scene* Main::createScene()
{
	Scene* scene = Scene::createWithPhysics();
    Main* layer = Main::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
	scene->addChild(layer);
	return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool Main::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	auto cclayer = CCLayerColor::create(ccc4(245, 245, 245, 255));
	DrawNode* drawNode = DrawNode::create();
    this->addChild(cclayer);
	this->addChild(drawNode);
	drawNode->drawSolidRect(Vec2(origin.x, origin.y + visibleSize.height),
		Vec2(origin.x + visibleSize.width, origin.y + visibleSize.height - 30), Color4F(0.3, 0.3, 0.3, 1.0));
    clientSize = cocos2d::Size(visibleSize.width, visibleSize.height - 30);
	scoreNum = Label::createWithTTF("Score: 0", "fonts/Marker Felt.ttf", 16);
    scoreNum->setPosition(Vec2(origin.x + visibleSize.width - scoreNum->getContentSize().width / 2 - 20,
        origin.y + visibleSize.height - 15));
	this->addChild(scoreNum);
// 	auto backItem = MenuItemImage::create("normal_backButton.png", "selected_backButton.png", CC_CALLBACK_1(Main::menuBackCallback, this));
// 	backItem->setPosition(Vec2(origin.x + backItem->getContentSize().width / 2 + 20,
// 		origin.y + visibleSize.height - 15));
// 	auto menu = Menu::create(backItem, NULL);
// 	menu->setPosition(Vec2::ZERO);
// 	this->addChild(menu);
    auto backButton = ui::Button::create("normal_backButton.png", "selected_backButton.png","disabled_backButton.png");
    backButton->setPosition(Vec2(origin.x + backButton->getContentSize().width / 2 + 20,
        origin.y + visibleSize.height - 15));
    backButton->addClickEventListener(CC_CALLBACK_1(Main::buttonBackCallback, this));
    this->addChild(backButton);
	auto edgeSpace = Sprite::create();
    auto boundBody = PhysicsBody::createEdgeBox(Size(clientSize.width, clientSize.height + 200));
    boundBody->getShape(0)->setRestitution(1.0f);
    boundBody->getShape(0)->setFriction(0.0f);
    boundBody->getShape(0)->setDensity(1.0f);
    boundBody->setCategoryBitmask(0x02);
    boundBody->setContactTestBitmask(0x00);
    boundBody->setCollisionBitmask(0x02);
	edgeSpace->setPhysicsBody(boundBody);
    edgeSpace->setPosition(Vec2(origin.x + clientSize.width / 2, origin.y + clientSize.height / 2 - 100));
	this->addChild(edgeSpace);
    platform = Sprite::create("platform.png");
    platform->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + platform->getContentSize().height));
    auto platformBody = PhysicsBody::createBox(platform->getContentSize());
    platformBody->setDynamic(false);
    platformBody->setGravityEnable(false);
    platformBody->getShape(0)->setRestitution(1.0f);
    platformBody->getShape(0)->setFriction(0.0f);
    platformBody->getShape(0)->setDensity(1.0f);
    platformBody->setCategoryBitmask(0x02);
    platformBody->setContactTestBitmask(0x00);
    platformBody->setCollisionBitmask(0x02);
    platform->setPhysicsBody(platformBody);
    this->addChild(platform);
    ball = Sprite::create("ball.png");
    ball->setPosition(platform->getPositionX(), 
        platform->getPositionY() + ball->getContentSize().height / 2 + platform->getContentSize().height / 2);
    ballBody = PhysicsBody::createCircle(ball->getContentSize().width / 2);
    ballBody->setGravityEnable(false);
	ballBody->getShape(0)->setRestitution(1.0f);
	ballBody->getShape(0)->setFriction(0.0f);
	ballBody->getShape(0)->setDensity(1.0f);
    /*ballBody->setVelocity(Vec2(100, 100));*/
	/*ballBody->applyImpulse(Vect(1000.0f, 1000.0f));*/
    ballBody->setCategoryBitmask(0x03);
    ballBody->setContactTestBitmask(0x05);
    ballBody->setCollisionBitmask(0x07);
    ball->setPhysicsBody(ballBody);
    ball->setTag(10);
    this->addChild(ball);
    srand((unsigned)time(NULL));
    int blocksNum= rand() % 51 + 50;
    for (int i = 0; i < blocksNum; ++i) {
        this->generateBlock(i, rand() % 3);
    }
	auto listener = EventListenerKeyboard::create();
	listener->onKeyPressed = CC_CALLBACK_2(Main::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(Main::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Main::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
    this->scheduleUpdate();
    return true;
}

void Main::LeftMovePlatform(float dt)
{
    if (platform->getPositionX() - platform->getContentSize().width / 2 > origin.x + 10) {
        platform->setPositionX(platform->getPositionX() - 10);
    }
}

void Main::RightMovePlatform(float dt)
{
    if (platform->getPositionX() + platform->getContentSize().width / 2 < origin.x + visibleSize.width - 10) {
        platform->setPositionX(platform->getPositionX() + 10);
    }
}

void Main::buttonBackCallback(Ref* pSender)
{
    Director::getInstance()->replaceScene(MainMenu::createScene());
}

void Main::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_A || keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
        schedule(schedule_selector(Main::LeftMovePlatform), 1.0f / 60);
        if (!ballActived) 
        {
            ballBody->setVelocity(Vec2(-100, 100));
            ballBody->applyImpulse(Vect(-10000.0f, 10000.0f));
            ballActived = true;
        }
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_D || keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
        schedule(schedule_selector(Main::RightMovePlatform), 1.0f / 60);
		if (!ballActived)
		{
			ballBody->setVelocity(Vec2(100, 100));
            ballBody->applyImpulse(Vect(10000.0f, 10000.0f));
			ballActived = true;
		}
    }
}

void Main::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    this->unschedule(schedule_selector(Main::LeftMovePlatform));
    this->unschedule(schedule_selector(Main::RightMovePlatform));
}

bool Main::onContactBegin(PhysicsContact& contact)
{
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();

	if (nodeA && nodeB)
	{
		if (nodeA->getTag() == 10)
		{
            DamageBlock(nodeB->getPosition());
		}
		else if (nodeB->getTag() == 10)
		{
            DamageBlock(nodeA->getPosition());
		}
	}
	return true;
}

void Main::generateBlock(int index, int status)
{
    const char* filename;
    switch (status)
    {
    case 0:filename = "blue_block.png"; break;
    case 1:filename = "green_block.png"; break;
    case 2:filename = "red_block.png"; break;
    default:return;
    }
    Sprite* block = Sprite::create(filename);
    auto blockBody = PhysicsBody::createBox(block->getContentSize());
    blockBody->setDynamic(false);
    blockBody->setGravityEnable(false);
    blockBody->getShape(0)->setRestitution(1.0f);
    blockBody->getShape(0)->setFriction(0.0f);
    blockBody->getShape(0)->setDensity(1.0f);
    blockBody->setCategoryBitmask(0x04);
    blockBody->setContactTestBitmask(0x01);
    blockBody->setCollisionBitmask(0x01);
    block->setPhysicsBody(blockBody);
    int maxRowNum = visibleSize.width / block->getContentSize().width;
    int offset = (visibleSize.width - maxRowNum * block->getContentSize().width) / 2;
    float rowPos = (index / maxRowNum) * block->getContentSize().width;
    float colPos = (index % maxRowNum) * block->getContentSize().height;
    block->setPosition(Vec2(origin.x + block->getContentSize().width / 2 + colPos + offset,
        origin.y + clientSize.height - block->getContentSize().height / 2 - rowPos));
    this->addChild(block);
    this->AddBlock(block, status);
}

void Main::AddBlock(Sprite* block, int status)
{
    this->blockList.push_back(block);
    this->blockPosition.push_back(block->getPosition());
    this->blockStatusList.push_back(status);
}

void Main::DamageBlock(Vec2 pos)
{
    int index = SearchIndexInArray(pos);
    if (index != -1) 
    {
        int status = --blockStatusList[index];
		const char* filename;
		switch (status)
		{
		case 0:filename = "blue_block.png"; break;
		case 1:filename = "green_block.png"; break;
		case 2:filename = "red_block.png"; break;
        default:
            blockList[index]->removeFromParent();
            char scoreNumStr[20];
            sprintf_s(scoreNumStr,"Scroe: %d", ++score);
            scoreNum->setString(scoreNumStr);
            break;
		}
        if (status >= 0)blockList[index]->setTexture(filename);
    }
}

int Main::SearchIndexInArray(Vec2 pos)
{
	for (int i = 0; i < this->blockPosition.size(); ++i)
	{
        if (pos == this->blockPosition[i]) return i;
	}
    return -1;
}

void Main::update(float dt) 
{
#ifdef DEBUG
    platform->setPositionX(ball->getPositionX());
#endif // DEBUG
    if (score == blockList.size()) 
    {
		this->unscheduleUpdate();
		ball->removeFromParentAndCleanup(true);
        Director::getInstance()->replaceScene(GameResult::createScene(true, score));
    }
    if (ball->getPositionY() < origin.y-ball->getContentSize().height / 2)
    {
		this->unscheduleUpdate();
		ball->removeFromParentAndCleanup(true);
		Director::getInstance()->replaceScene(GameResult::createScene(false, score));
	}
}