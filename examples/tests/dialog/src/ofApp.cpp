#include "ofApp.h"

void ofApp::setup() {
}

void ofApp::update(){

}

void ofApp::draw(){

}

void ofApp::keyPressed(int key){
	if (key == '1') {
		ofSystemAlertDialog("ARWIL");
	}
	else if (key == '2') {
		auto res { ofSystemLoadDialog("titulo da janela", true, "~/Desktop") };
		if (res.bSuccess) {
			cout << res.fileName << endl;
		} else {
			cout << "FAIL" << endl;
		}
	}
	
	else if (key == '3') {
		auto res { ofSystemLoadDialog("titulo da janela", false, "~/Desktop") };
		if (res.bSuccess) {
			cout << res.fileName << endl;
		} else {
			cout << "FAIL" << endl;
		}
	}
	
	else if (key == '4') {
		auto res { ofSystemSaveDialog("default Name", "message name") };
		if (res.bSuccess) {
			cout << res.fileName << endl;
			cout << res.filePath << endl;
		} else {
			cout << "FAIL" << endl;
		}
	}
	
	else if (key == '5') {
		auto res { ofSystemTextBoxDialog("Quantos você quer?", "três?") };
		cout << res << endl;
	}
	
	/// \brief show an error message in an alert dialog box
//	void ofSystemAlertDialog(std::string errorMessage);
//	ofFileDialogResult ofSystemLoadDialog(std::string windowTitle="", bool bFolderSelection = false, std::string defaultPath="");
//
//	/// \brief show a file save dialog box
//	/// \param defaultName suggested filename to start dialog, ie "screenshot.png"
//	/// \param messageName descriptive text for the save action, ie. "Saving screenshot as"
//	/// \return dialog result with selection (if any)
//	ofFileDialogResult ofSystemSaveDialog(std::string defaultName, std::string messageName);
//
//	/// \brief show a text entry dialog box
//	/// \param question descriptive text for the text entry, ie. "What's your favorite color?"
//	/// \param text optional default text entry string, ie. "blue"
//	std::string ofSystemTextBoxDialog(std::string question, std::string text="");


}

