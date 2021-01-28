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
#include "MainMenuScene.h"
#include "MainScene.h"
#include "GameResultScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* GameResult::createScene(bool isVictory, int score)
{
    auto scene = GameResult::create();
    scene->isVictory = isVictory;
    scene->scoreNum = score;
    scene->initialize();
    return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
void GameResult::initialize()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto cclayer = CCLayerColor::create(ccc4(34, 139, 34, 255));
    this->addChild(cclayer);
    char* messageStr;
    if (isVictory)messageStr = "You Win!";
    else messageStr = "Game over.";
    auto message = Label::createWithTTF(messageStr, "fonts/Marker Felt.ttf", 64);
    message->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height - message->getContentSize().height));
    this->addChild(message);
    char scoreStr[20];
    sprintf_s(scoreStr, "Score: %d", scoreNum);
    auto score= Label::createWithTTF(scoreStr, "fonts/Marker Felt.ttf", 24);
    score->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(score);
    auto restartButton = ui::Button::create();
    restartButton->setTitleText("Restart");
    restartButton->setPosition(Vec2(origin.x + visibleSize.width / 2 - 75, origin.y + 75));
    restartButton->addClickEventListener(CC_CALLBACK_1(GameResult::buttonRestartClick, this));
    this->addChild(restartButton);
	auto backButton = ui::Button::create();
    backButton->setTitleText("Back");
    backButton->setPosition(Vec2(origin.x + visibleSize.width / 2 + 75, origin.y + 75));
    backButton->addClickEventListener(CC_CALLBACK_1(GameResult::buttonBackClick, this));
	this->addChild(backButton);
}

void GameResult::buttonRestartClick(Ref* pSender)
{
    Director::getInstance()->replaceScene(Main::createScene());
}

void GameResult::buttonBackClick(Ref* pSender)
{
    Director::getInstance()->replaceScene(MainMenu::createScene());
}