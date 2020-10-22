#include "Game.h"

#include <cassert>
#include <Windows.h>

#include "ResourceManager/ResourceManager.h"
#include "Utils/Constants.h"



void Game::Run()
{
    GameInit();

    sf::Clock clock;
    sf::Time dt; 
    while (m_Window.isOpen())
    {
        dt = clock.restart();
        Update(dt.asSeconds());
        Render();
    }

    GameRelease();
}

void Game::GameInit()
{
    m_GameConfig.ReadFromFile(SETTINGS_FILE_PATH);

    ResourceManager::PreloadTexturesFromFolder(TEXTURES_FOLDER_PATH);

    if (m_GameConfig.isFullscreen)
    {
        if (m_GameConfig.fullscreenMode.isValid())
        {
            m_Window.create(m_GameConfig.fullscreenMode, WINDOW_NAME, sf::Style::Fullscreen);
        }
        else
        {
            const std::vector<sf::VideoMode>& videoModes = sf::VideoMode::getFullscreenModes();
            assert(!videoModes.empty() && "There should be available video mode");
            m_Window.create(videoModes[0], WINDOW_NAME, sf::Style::Fullscreen);
        }
    }
    else
    {
        m_Window.create(m_GameConfig.windowedMode, WINDOW_NAME, sf::Style::Default);
    }

    m_TestSprite.setTexture(ResourceManager::GetTexture("testTexture"));
}

void Game::Update(float dt)
{
    sf::Event event;
    while (m_Window.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
        {
            m_Window.close();
            break;
        }
        case sf::Event::Resized:
        {
            if (!m_GameConfig.isFullscreen)
            {
                m_GameConfig.windowedMode = sf::VideoMode(event.size.width, event.size.height);
                m_GameConfig.WriteToFile(SETTINGS_FILE_PATH);
            }
            break;
        }
        }  
    }
}

void Game::Render()
{
    m_Window.clear();

    m_Window.draw(m_TestSprite);

    m_Window.display();
}

void Game::GameRelease()
{
    ResourceManager::ReleaseResources();
}

void Game::GameConfig::WriteToFile(const std::string& filePath) const
{
    std::string buffer;

    buffer = std::to_string(fullscreenMode.width);
    WritePrivateProfileStringA("Window", "FullscreenWidth", buffer.c_str(), filePath.c_str());

    buffer = std::to_string(fullscreenMode.height);
    WritePrivateProfileStringA("Window", "FullscreenHeight", buffer.c_str(), filePath.c_str());

    buffer = std::to_string(windowedMode.width);
    WritePrivateProfileStringA("Window", "WindowedWidth", buffer.c_str(), filePath.c_str());

    buffer = std::to_string(windowedMode.height);
    WritePrivateProfileStringA("Window", "WindowedHeight", buffer.c_str(), filePath.c_str());

    buffer = std::to_string(static_cast<int>(isFullscreen));
    WritePrivateProfileStringA("Window", "IsFullscreen", buffer.c_str(), filePath.c_str());
}

void Game::GameConfig::ReadFromFile(const std::string& filePath)
{
    const int fullscreenWidth = GetPrivateProfileIntA("Window", "FullscreenWidth", DEFAULT_WINDOW_WIDTH, filePath.c_str());
    const int fullscreenHeight = GetPrivateProfileIntA("Window", "FullscreenHeight", DEFAULT_WINDOW_HEIGHT, filePath.c_str());
    const int windowedWidth = GetPrivateProfileIntA("Window", "WindowedWidth", DEFAULT_WINDOW_WIDTH, filePath.c_str());
    const int windowedHeight = GetPrivateProfileIntA("Window", "WindowedHeight", DEFAULT_WINDOW_HEIGHT, filePath.c_str());
    fullscreenMode = sf::VideoMode(fullscreenWidth, fullscreenHeight);
    windowedMode = sf::VideoMode(windowedWidth, windowedHeight);
    isFullscreen = GetPrivateProfileIntA("Window", "IsFullscreen", DEFAULT_FULLSCREEN, filePath.c_str());
}
