#include "NesPortConfig.h"
#include <qfiledialog.h>
#include <QTimer>
#include <QDebug>
#include <string>
#include <sstream>

using namespace std;

NesPortConfig::NesPortConfig(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	if (SDL_Init(SDL_INIT_JOYSTICK) == 0) {
		qDebug() << "SDL_Init Error:" << SDL_GetError();
	}

	sdlWindow = SDL_CreateWindow("Waiting for input", 300, 1, SDL_WINDOW_HIDDEN | SDL_WINDOW_UTILITY);
	if (!sdlWindow) {
		qDebug() << "SDL_CreateWindow Error:" << SDL_GetError();
	}

	//check for joysticks
	int joyStickCnt;
	SDL_JoystickID* joySticks;

	joySticks = SDL_GetJoysticks(&joyStickCnt);
	for (int i = 0; i < joyStickCnt; i++) {
		SDL_Joystick* j = SDL_OpenJoystick(joySticks[i]);
		if (j) js.push_back(j);
	}
	SDL_free(joySticks);

	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &NesPortConfig::pollSdlEvents);
	timer->start(10); // Poll every 10 milliseconds

	loadConfigFile("");
}

NesPortConfig::~NesPortConfig()
{
	for (int i = 0; i < js.size(); i++) {
		SDL_CloseJoystick(js[i]);
	}
	if (sdlWindow) {
		SDL_DestroyWindow(sdlWindow);
	}
	SDL_Quit();
}

void NesPortConfig::on_gamePathSelect_clicked() {
    QString filename = QFileDialog::getExistingDirectory(this, "Select the game folder", "");
    ui.gamePath->setText(filename);
    loadConfigFile(filename);
}

void NesPortConfig::loadConfigFile(QString path) {
	string configPath = path.toStdString();
	string lastChar = "";
	if (configPath.length() > 0) {
		lastChar = configPath.substr(configPath.length() - 1, 1);
	}
	if (lastChar == "/" || lastChar == "\\") {
		configPath += "config.txt";
	}
	else {
	}
    char* configData;
    size_t dataSize;
    configData = (char*)SDL_LoadFile(configPath.c_str(), &dataSize);
    if (configData) {
		string tmpStr = configData;
		tmpStr.erase(std::remove(tmpStr.begin(), tmpStr.end(), '\r'), tmpStr.end());
		stringstream s(tmpStr);
		string configLine;
		while (getline(s, configLine)) {
			size_t findIdx = configLine.find(":");
			if (findIdx > 0) {
				string lineHead = configLine.substr(0, findIdx);
				string lineTail = configLine.substr(findIdx + 1);
				QString lineStr = QString::fromStdString(lineTail);
				if (lineHead == "ROM") {
					ui.romFileName->setText(lineStr);
				}
				else {
					size_t findIdx2 = lineTail.find("=");
					string valueHead = lineTail.substr(0, findIdx2);
					string valueTail = lineTail.substr(findIdx2 + 1);
					lineStr = QString::fromStdString(valueTail);
					if (lineHead == "DISPLAY") {
						if (valueHead == "PALETTE") {
							ui.paletteFileName->setText(lineStr);
						}
						else if (valueHead == "WIDTH") {
							ui.displayWidth->setText(lineStr);
						}
						else if (valueHead == "HEIGHT") {
							ui.displayHeight->setText(lineStr);
						}
					}
					else if(lineHead == "CONTROL") {
						if (valueHead == "P1_UP") {
							ui.p1UpLabel->setText(lineStr);
							inputSettings[0][0] = valueTail;
						}
						else if (valueHead == "P1_DOWN") {
							ui.p1DownLabel->setText(lineStr);
							inputSettings[0][1] = valueTail;
						}
						else if (valueHead == "P1_LEFT") {
							ui.p1LeftLabel->setText(lineStr);
							inputSettings[0][2] = valueTail;
						}
						else if (valueHead == "P1_RIGHT") {
							ui.p1RightLabel->setText(lineStr);
							inputSettings[0][3] = valueTail;
						}
						else if (valueHead == "P1_A") {
							ui.p1ALabel->setText(lineStr);
							inputSettings[0][4] = valueTail;
						}
						else if (valueHead == "P1_B") {
							ui.p1BLabel->setText(lineStr);
							inputSettings[0][5] = valueTail;
						}
						else if (valueHead == "P1_SELECT") {
							ui.p1SelectLabel->setText(lineStr);
							inputSettings[0][6] = valueTail;
						}
						else if (valueHead == "P1_START") {
							ui.p1StartLabel->setText(lineStr);
							inputSettings[0][7] = valueTail;
						}
						else if (valueHead == "P2_UP") {
							ui.p2UpLabel->setText(lineStr);
							inputSettings[1][0] = valueTail;
						}
						else if (valueHead == "P2_DOWN") {
							ui.p2DownLabel->setText(lineStr);
							inputSettings[1][1] = valueTail;
						}
						else if (valueHead == "P2_LEFT") {
							ui.p2LeftLabel->setText(lineStr);
							inputSettings[1][2] = valueTail;
						}
						else if (valueHead == "P2_RIGHT") {
							ui.p2RightLabel->setText(lineStr);
							inputSettings[1][3] = valueTail;
						}
						else if (valueHead == "P2_A") {
							ui.p2ALabel->setText(lineStr);
							inputSettings[1][4] = valueTail;
						}
						else if (valueHead == "P2_B") {
							ui.p2BLabel->setText(lineStr);
							inputSettings[1][5] = valueTail;
						}
						else if (valueHead == "P2_SELECT") {
							ui.p2SelectLabel->setText(lineStr);
							inputSettings[1][6] = valueTail;
						}
						else if (valueHead == "P2_START") {
							ui.p2StartLabel->setText(lineStr);
							inputSettings[1][7] = valueTail;
						}
					}
				}
			}
		}
		SDL_free(configData);
    }
}

void NesPortConfig::on_romFileSelect_clicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Select ROM file", "", "ROM Files (*.nes)");
    ui.romFileName->setText(filename);
}

void NesPortConfig::on_palFileSelect_clicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Select palette file", "", "Palette Files (*.pal)");
    ui.paletteFileName->setText(filename);
}

void NesPortConfig::setWaitForInput(string id) {
	string t = "Waiting for " + id + " input...";
	SDL_SetWindowTitle(sdlWindow, t.c_str());
	SDL_ShowWindow(sdlWindow);
	SDL_RestoreWindow(sdlWindow);
	SDL_RaiseWindow(sdlWindow);
	inputID = id;
}


void NesPortConfig::on_p1Up_clicked() {
	setWaitForInput("P1 Up");
}

void NesPortConfig::on_p1Down_clicked() {
	setWaitForInput("P1 Down");
}

void NesPortConfig::on_p1Left_clicked() {
	setWaitForInput("P1 Left");
}

void NesPortConfig::on_p1Right_clicked() {
	setWaitForInput("P1 Right");
}

void NesPortConfig::on_p1A_clicked() {
	setWaitForInput("P1 A");
}

void NesPortConfig::on_p1B_clicked() {
	setWaitForInput("P1 B");
}

void NesPortConfig::on_p1Select_clicked() {
	setWaitForInput("P1 Select");
}

void NesPortConfig::on_p1Start_clicked() {
	setWaitForInput("P1 Start");
}


void NesPortConfig::on_p2Up_clicked() {
	setWaitForInput("P2 Up");
}

void NesPortConfig::on_p2Down_clicked() {
	setWaitForInput("P2 Down");
}

void NesPortConfig::on_p2Left_clicked() {
	setWaitForInput("P2 Left");
}

void NesPortConfig::on_p2Right_clicked() {
	setWaitForInput("P2 Right");
}

void NesPortConfig::on_p2A_clicked() {
	setWaitForInput("P2 A");
}

void NesPortConfig::on_p2B_clicked() {
	setWaitForInput("P2 B");
}

void NesPortConfig::on_p2Select_clicked() {
	setWaitForInput("P2 Select");
}

void NesPortConfig::on_p2Start_clicked() {
	setWaitForInput("P2 Start");
}


void NesPortConfig::pollSdlEvents() {
	SDL_Event event;
	inputSetting i = {0, 0};
	while (SDL_PollEvent(&event)) {
		// Handle SDL events based on event.type
		switch (event.type) {
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			SDL_HideWindow(sdlWindow);
			break;
		case SDL_EVENT_KEY_DOWN:
			i.type = 1;
			i.v = event.key.scancode;
			inputReceived(i);
			break;
		case SDL_EVENT_JOYSTICK_AXIS_MOTION:
			i.type = 2;
			i.v = (event.jaxis.axis << 8) | (event.jaxis.value < 0 ? 0xFF : 0x00);
			inputReceived(i);
			break;
		case SDL_EVENT_JOYSTICK_HAT_MOTION:
			i.type = 3;
			i.v = (event.jhat.hat << 8) | event.jhat.value;
			inputReceived(i);
			break;
		case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
			i.type = 4;
			i.v = event.jbutton.button;
			inputReceived(i);
			break;
		}
	}
}

void NesPortConfig::inputReceived(inputSetting i) {
	SDL_HideWindow(sdlWindow);
	string s;
	s = std::to_string(i.type) + "_" + std::to_string(i.v);
	QString lineStr = QString::fromStdString(s);
	if (inputID == "P1 Up") {
		inputSettings[0][0] = s;
		ui.p1UpLabel->setText(lineStr);
	}
	else if(inputID == "P1 Down") {
		inputSettings[0][1] = s;
		ui.p1DownLabel->setText(lineStr);
	}
	else if (inputID == "P1 Left") {
		inputSettings[0][2] = s;
		ui.p1LeftLabel->setText(lineStr);
	}
	else if (inputID == "P1 Right") {
		inputSettings[0][3] = s;
		ui.p1RightLabel->setText(lineStr);
	}
	else if (inputID == "P1 A") {
		inputSettings[0][4] = s;
		ui.p1ALabel->setText(lineStr);
	}
	else if (inputID == "P1 B") {
		inputSettings[0][5] = s;
		ui.p1BLabel->setText(lineStr);
	}
	else if (inputID == "P1 Select") {
		inputSettings[0][6] = s;
		ui.p1SelectLabel->setText(lineStr);
	}
	else if (inputID == "P1 Start") {
		inputSettings[0][7] = s;
		ui.p1StartLabel->setText(lineStr);
	}
	else if (inputID == "P2 Up") {
		inputSettings[1][0] = s;
		ui.p2UpLabel->setText(lineStr);
	}
	else if (inputID == "P2 Down") {
		inputSettings[1][1] = s;
		ui.p2DownLabel->setText(lineStr);
	}
	else if (inputID == "P2 Left") {
		inputSettings[1][2] = s;
		ui.p2LeftLabel->setText(lineStr);
	}
	else if (inputID == "P2 Right") {
		inputSettings[1][3] = s;
		ui.p2RightLabel->setText(lineStr);
	}
	else if (inputID == "P2 A") {
		inputSettings[1][4] = s;
		ui.p2ALabel->setText(lineStr);
	}
	else if (inputID == "P2 B") {
		inputSettings[1][5] = s;
		ui.p2BLabel->setText(lineStr);
	}
	else if (inputID == "P2 Select") {
		inputSettings[1][6] = s;
		ui.p2SelectLabel->setText(lineStr);
	}
	else if (inputID == "P2 Start") {
		inputSettings[1][7] = s;
		ui.p2StartLabel->setText(lineStr);
	}
	inputID = "";
}

void NesPortConfig::on_saveConfig_clicked() {
	string configPath = ui.gamePath->text().toStdString() + "/config.txt";
	SDL_IOStream* stream = SDL_IOFromFile(configPath.c_str(), "w");
	
	string line;
	line = "ROM:" + ui.romFileName->text().toStdString() + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "DISPLAY:PALETTE=" + ui.paletteFileName->text().toStdString() + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "DISPLAY:WIDTH=" + ui.displayWidth->text().toStdString() + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "DISPLAY:HEIGHT=" + ui.displayHeight->text().toStdString() + "\n";

	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P1_UP=" + inputSettings[0][0] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P1_DOWN=" + inputSettings[0][1] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P1_LEFT=" + inputSettings[0][2] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P1_RIGHT=" + inputSettings[0][3] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P1_A=" + inputSettings[0][4] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P1_B=" + inputSettings[0][5] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P1_SELECT=" + inputSettings[0][6] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P1_START=" + inputSettings[0][7] + "\n";
	SDL_IOprintf(stream, line.c_str());

	line = "CONTROL:P2_UP=" + inputSettings[1][0] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P2_DOWN=" + inputSettings[1][1] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P2_LEFT=" + inputSettings[1][2] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P2_RIGHT=" + inputSettings[1][3] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P2_A=" + inputSettings[1][4] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P2_B=" + inputSettings[1][5] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P2_SELECT=" + inputSettings[1][6] + "\n";
	SDL_IOprintf(stream, line.c_str());
	line = "CONTROL:P2_START=" + inputSettings[1][7] + "\n";
	SDL_IOprintf(stream, line.c_str());

	SDL_CloseIO(stream);
}