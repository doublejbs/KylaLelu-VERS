#include "ofApp.h"

int cam_w;
int cam_h;
int target_w;
int target_h;

//--------------------------------------------------------------
void ofApp::setup(){
    cam_w = 320;
    cam_h = 240;
    target_w = 200;
    target_h = 200;
    
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(cam_w,cam_h);
    vidGrabber.setup(cam_w,cam_h);
    
    colorImg_cam.allocate(cam_w,cam_h);
    colorImg.allocate(target_w,target_h);
    grayImage.allocate(target_w,target_h);
    grayBg.allocate(target_w,target_h);
    grayDiff.allocate(target_w,target_h);
    
    bLearnBakground = true;
    threshold = 80;
    ofGetWidth();
}

//--------------------------------------------------------------
void ofApp::update(){
    ofBackground(100,100,100);
    
    bool bNewFrame = false;
    
    vidGrabber.update();
    bNewFrame = vidGrabber.isFrameNew();
    
    if (bNewFrame){
        img.setFromPixels(vidGrabber.getPixels());
        colorImg_cam.setFromPixels(img.getPixels());
        img.crop((cam_w-target_w)/2, (cam_h-target_h)/2, target_w, target_h);
        colorImg.setFromPixels(img.getPixels());
        grayImage = colorImg;
        
        if (bLearnBakground == true){
            grayBg = grayImage;
            bLearnBakground = false;
        }
        
        grayDiff.absDiff(grayBg, grayImage);
        grayDiff.threshold(threshold);
        
        contourFinder.findContours(grayDiff, 20, target_w*target_h/2, 100, true);
        printf("value %d \n", contourFinder.nBlobs);
        
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    int ini_w = 50;
    int ini_h = 50;
    
    ofSetHexColor(0xffffff);
    colorImg_cam.draw(ini_w, ini_h);
    grayImage.draw(ini_w+(cam_w-target_w)/2, ini_h+(cam_h-target_h)/2);
    
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(ini_w+(cam_w-target_w)/2, ini_h+(cam_h-target_h)/2);
    }
    
    ofSetHexColor(0xffffff);
    stringstream reportStr;
    reportStr << "bg subtraction and blob detection" << endl
    << "press ' ' to capture bg" << endl
    << "threshold " << threshold << " (press: +/-)" << endl
    << "num blobs found " << contourFinder.nBlobs << ", fps: " << ofGetFrameRate();
    ofDrawBitmapString(reportStr.str(), ini_w, ini_h+cam_h+50);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key){
        case ' ':
            bLearnBakground = true;
            break;
        case '+':
            threshold ++;
            if (threshold > 255) threshold = 255;
            break;
        case '-':
            threshold --;
            if (threshold < 0) threshold = 0;
            break;
    }
}
