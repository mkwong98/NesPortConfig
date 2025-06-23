#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NesPortConfig.h"
#include <string>
#include <vector>
#include <SDL3/SDL.h>

using namespace std;

struct inputSetting {
    Uint8 type; //0=none, 1=keyboard, 2=joystick axis, 3=joystick hat, 4=joystick button
    Uint32 id;
    Uint32 v;
};

class NesPortConfig : public QMainWindow
{
    Q_OBJECT

public:
    NesPortConfig(QWidget *parent = nullptr);
    ~NesPortConfig();

private:
    Ui::NesPortConfigClass ui;
    SDL_Window* sdlWindow = nullptr;
    int joyStickCnt;
    SDL_JoystickID* joySticks;
    vector<SDL_Joystick*>js;
    string inputID;
    string inputSettings[2][8];

    void loadConfigFile(QString path);
    void setWaitForInput(string id);
    void inputReceived(inputSetting i);

private slots:
    void on_gamePathSelect_clicked();
    void on_romFileSelect_clicked();
    void on_palFileSelect_clicked();

    void on_p1Up_clicked();
    void on_p1Down_clicked();
    void on_p1Left_clicked();
    void on_p1Right_clicked();
    void on_p1A_clicked();
    void on_p1B_clicked();
    void on_p1Select_clicked();
    void on_p1Start_clicked();

    void on_p2Up_clicked();
    void on_p2Down_clicked();
    void on_p2Left_clicked();
    void on_p2Right_clicked();
    void on_p2A_clicked();
    void on_p2B_clicked();
    void on_p2Select_clicked();
    void on_p2Start_clicked();

    void pollSdlEvents();

    void on_saveConfig_clicked();
};

