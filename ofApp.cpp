#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);

	this->box2d.init();
	this->box2d.setGravity(0, 50);
	this->box2d.createBounds();
	this->box2d.setFPS(30);

	this->cap.open("D:\\video\\image41.mp4");
	this->cap_size = ofVec2f(this->cap.get(CV_CAP_PROP_FRAME_WIDTH), this->cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	this->cap_size *= 0.1;

	this->image.allocate(this->cap_size.x, this->cap_size.y, OF_IMAGE_COLOR);
	this->frame = cv::Mat(this->image.getHeight(), this->image.getWidth(), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);
}

//--------------------------------------------------------------
void ofApp::update() {

	cv::Mat src;
	this->cap >> src;

	if (src.empty()) {

		this->cap.set(CV_CAP_PROP_POS_FRAMES, 1);
		return;
	}

	cv::cvtColor(src, src, cv::COLOR_BGR2RGB);
	cv::resize(src, this->frame, cv::Size(this->cap_size.x, this->cap_size.y));

	float span = 15;

	if (ofGetFrameNum() > 1) {

		cv::Mat gap = this->pre_frame - this->frame;

		for (int y = 0; y < gap.rows; y += span) {

			cv::Vec3b* gap_value = gap.ptr<cv::Vec3b>(y, 0);
			cv::Vec3b* color_value = this->frame.ptr<cv::Vec3b>(y, 0);
			for (int x = 0; x < gap.cols; x += span) {

				cv::Vec3b gv = gap_value[x];
				cv::Vec3b cv = color_value[x];
				if ((gv[0] + gv[1] + gv[2]) > 32 && cv[0] < 100) {

					auto circle = make_shared<ofxBox2dCircle>();
					circle.get()->setPhysics(3.0, 0.5, 0.1);
					circle.get()->setup(this->box2d.getWorld(), x + ofGetWidth() / 2 - this->image.getWidth() / 2 + ofRandom(-span / 2, span / 2), y + 100 + ofRandom(-span / 2, span / 2), span / 3);
					this->circles.push_back(move(circle));

					this->circles_life.push_back(255);

					ofColor c;
					c.setHsb(ofRandom(255), 200, 255);
					this->circles_color.push_back(c);

				}
			}
		}
	}

	this->frame.copyTo(this->pre_frame);

	this->image.update();
	this->box2d.update();
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofSetColor(255);
	this->image.draw(ofGetWidth() / 2 - this->image.getWidth() / 2, 100);

	for (int i = this->circles.size() - 1; i >= 0; i--) {

		this->circles_life[i] -= 4;
		if (this->circles_life[i] < 0) {

			this->circles[i].get()->destroy();
			this->circles.erase(this->circles.begin() + i);
			this->circles_life.erase(this->circles_life.begin() + i);
			this->circles_color.erase(this->circles_color.begin() + i);
		}
		else {

			ofSetColor(this->circles_color[i]);
			ofDrawCircle(this->circles[i].get()->getPosition(), this->circles[i].get()->getRadius());
			ofDrawLine(this->circles[i].get()->getPosition(), this->circles[i].get()->getPosition() - this->circles[i].get()->getVelocity());
		}
	}
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}