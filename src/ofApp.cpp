#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetRectMode(OF_RECTMODE_CENTER);
	ofSetBackgroundColor(10);
	ofSetFrameRate(24);

	ofTrueTypeFont::setGlobalDpi(72);
	// setup title font
	title.load("Blanka-Regular.ttf", 50, true, true);
	title.setLineHeight(lineHeight);
	title.setLetterSpacing(letterSpacing);
	// setup instructions title font
	instructionsTitle.load("Blanka-Regular.ttf", 50, true, true);
	instructionsTitle.setLineHeight(lineHeight);
	instructionsTitle.setLetterSpacing(letterSpacing);
	// setup instructions font
	instructions.load("Blanka-Regular.ttf", 20, true, true);
	instructions.setLineHeight(lineHeight);
	instructions.setLetterSpacing(letterSpacing);
	// setup start button
	startFont.load("Blanka-Regular.ttf", 30, true, true);
	startFont.setLineHeight(lineHeight);
	startFont.setLetterSpacing(letterSpacing);
	startButton = ofRectangle(ofGetWidth() / 2, ofGetHeight() / 2 + 200, 200, 65);
	// setup game result font
	gameResult.load("Blanka-Regular.ttf", 75, true, true);
	gameResult.setLineHeight(lineHeight);
	gameResult.setLetterSpacing(letterSpacing);

	// setup game intro music
	introAudio.load("Audio/introMusic.mp3");
	introAudio.setVolume(0.75);
	// setup game start audio
	gameStartAudio.load("Audio/gameStartAudio.mp3");
	gameStartAudio.setVolume(0.75);
	// setup background music 
	backgroundAudio.load("Audio/backgroundMusic.mp3");
	backgroundAudio.setVolume(0.75);
	backgroundAudio.setLoop(true);
	// setup victory audio
	victoryAudio.load("Audio/victoryAudio.mp3");
	victoryAudio.setVolume(0.75);
	// setup game over audio
	gameOverAudio.load("Audio/gameOverAudio.mp3");
	gameOverAudio.setVolume(0.75);
}

//--------------------------------------------------------------
void ofApp::update() {
	if (gameState == GameState::playing) {
		manageLoseCondition();
		manageWinCondition();
		cleanUpProjectiles();
		alienSwarm.update();
		alienSwarm.loadProjectile(attackProbability);
		manageAlienCollisions();
		manageHeroCollisions();
	}
	// update the sound playing system
	ofSoundUpdate();

}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(225);
	
	// draw border lines
	const int offsetX = 30;
	ofDrawLine(offsetX, upperBoundary, ofGetWidth() - offsetX, upperBoundary);
	ofDrawLine(offsetX, lowerBoundary, ofGetWidth() - offsetX, lowerBoundary);

	// draw game title; align game title to the centre
	const ofRectangle titleBounds = title.getStringBoundingBox(titleStr, 0, 0);
	const float titlePosX = ofGetWidth() / 2 - titleBounds.width / 2;
	const float titlePosY = upperBoundary / 2 + titleBounds.height / 2;
	title.drawString(titleStr, titlePosX, titlePosY);
	ofDrawRectangle(titleBounds);

	// draw score
	player.drawScore();
	// draw health
	player.drawHealth();

	// handle game states
	switch(gameState) {
		case GameState::startScreen: 
			// draw game instructions
			drawInstruction();
			// draw start button
			drawStartButton();
			// player intro music
			if(!introAudio.isPlaying()) introAudio.play();
			break;
		case GameState::playing: 
			// draw player
			player.drawPlayer();
			// draw player projectiles
			player.drawProjectiles();
			// draw alien swarm
			alienSwarm.draw();
			// draw enemy projectiles
			alienSwarm.drawProjectiles();
			break;
		case GameState::won:
			// stop background music
			backgroundAudio.stop();
			// play victory audio
			if (!victoryAudio.isPlaying() && !isVictoryAudioPlayed) {
				victoryAudio.play();
				isVictoryAudioPlayed = true;
			}
			// draw player
			player.drawPlayer();
			// draw end screen
			drawEndScreen();
			break;
		case GameState::lost:
			// stop background music
			backgroundAudio.stop();
			// play game over audio
			if (!gameOverAudio.isPlaying() && !isGameOverAudioPlayed) {
				gameOverAudio.play();
				isGameOverAudioPlayed = true;
			}
			// draw alien swarm
			alienSwarm.draw();
			// draw end screen
			drawEndScreen();
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (gameState == GameState::playing) {
		if (key == 'a') {
			player.updateCoordinateX(-heroMovementSpeed, leftBoundary, rightBoundary);
		}
		if (key == 'd') {
			player.updateCoordinateX(heroMovementSpeed, leftBoundary, rightBoundary);
		}
		if (key == 'w') {
			player.addProjectile();
		}
	} 
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	if(gameState == GameState::playing) {
		player.setCoordinateX(x, leftBoundary, rightBoundary);
	} else if (gameState == GameState::startScreen) {
		if (startButton.inside(x + startButton.width / 2, y + startButton.height / 2)) {
			startButtonColor = ofColor::darkGrey;
			isButtonHovered = true;
		} else {
			startButtonColor = ofColor::white;
			isButtonHovered = false;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (gameState == GameState::playing) {
		player.addProjectile();
	} else if (gameState == GameState::startScreen) {
		if(isButtonHovered) {
			gameStartAudio.play();
			backgroundAudio.play();
			gameState = GameState::playing;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::drawInstruction() const {
	const ofRectangle instrBounds = instructions.getStringBoundingBox(instructionsStr, ofGetWidth() / 2, ofGetHeight() / 2);
	// since the text is drawn from the bottom left corner, subtract an offset to center the text
	const float instrPosX = ofGetWidth() / 2 - instrBounds.width / 2;
	const float instrPosY = ofGetHeight() / 2 - instrBounds.height / 2;
	// draw a border around the instructions
	ofSetColor(255);
	ofDrawRectangle(instrBounds.x, instrBounds.y, ofGetWidth() - 60, instrBounds.height * 1.1);
	// draw instructions text
	ofSetColor(10);
	instructions.drawString(instructionsStr, instrPosX, instrPosY);

	const ofRectangle instrTitleBounds = instructionsTitle.getStringBoundingBox(instructionsTitleStr, 0, 0);
	// align text to the center
	const float instrTitlePosX = ofGetWidth() / 2 - instrTitleBounds.width / 2;
	const float instrTitlePosY = (instrPosY + upperBoundary) / 2;
	// draw instruction title text
	ofSetColor(255);
	instructionsTitle.drawString(instructionsTitleStr, instrTitlePosX, instrTitlePosY);
}

void ofApp::drawStartButton() const {
	ofRectangle startBounds = startFont.getStringBoundingBox(startStr, 0, 0);
	// align text to the center
	const float startStrPosX = startButton.x - startBounds.width / 2;
	const float startStrPosY = startButton.y + startBounds.height / 2;
	// draw button
	ofSetColor(startButtonColor);
	ofDrawRectangle(startButton);
	// draw button text
	ofSetColor(10);
	startFont.drawString(startStr, startStrPosX, startStrPosY);
}

void ofApp::drawEndScreen() const {
	std::string outcomeStr;
	if(gameState == GameState::lost) {
		outcomeStr = gameOverStr;
	}
	else if (gameState == GameState::won) {
		outcomeStr = victoryStr;
	}
	const ofRectangle resultBounds = instructions.getStringBoundingBox(outcomeStr, ofGetWidth() / 2, ofGetHeight() / 2);
	// align text to the center
	const float resultBoundsPosX = ofGetWidth() / 2 - resultBounds.width * 1.75;
	const float resultBoundsPosY = ofGetHeight() / 2 + resultBounds.height;
	// draw game outcome text
	ofSetColor(255);
	gameResult.drawString(outcomeStr, resultBoundsPosX, resultBoundsPosY);
}

void ofApp::cleanUpProjectiles() {
	// clean up projectiles outside the game boundaries
	player.cleanUpProjectiles(upperBoundary);
	alienSwarm.cleanUpProjectiles(lowerBoundary);
}

void ofApp::manageAlienCollisions() {
	// check if any player projectile and alien collisions are intersecting
	// destroy any that do
	for (int n{ 0 }; n < alienRow; n++) {
		for (int m{ 0 }; m < alienColumn; m++) {
			for (int j{ 0 }; j < player.getProjectiles().size(); j++) {
				if (player.isProjectileOverlapping(j, alienSwarm.getAlienCollision(n, m)) && alienSwarm.isAlienAlive(n, m)) {
					player.updateScore(alienSwarm.getAlienScore(n, m));
					player.destroyProjectile(j);
					alienSwarm.destroyAlien(n, m);
				}
			}
		}
	}
}

void ofApp::manageHeroCollisions() {
	// check if any alien projectile hit the player
	// destroy the projectile and damage the player if so
	for (size_t i{ 0 }; i < alienSwarm.getNumProjectile(); i++) {
		if (player.isOverlapping(alienSwarm.getProjectileCollision(i))) {
			player.updateHealth(enemyProjectileDamage);
			alienSwarm.destroyProjectile(i);
		}
	}
}

void ofApp::manageWinCondition() {
	// player wins if all the aliens have been destroyed
	if(alienSwarm.isDestroyed()) {
		gameState = GameState::won;
	}
}
void ofApp::manageLoseCondition() {
	// player loses if its life reaches 0
	// player loses if the alien swarm has reached its location (minus a small offset to account for the sprite height)
	const int loseBoundary{ static_cast<int>(heroCoordinateY - 50) }; 
	if (player.isDead() || alienSwarm.hasReached(loseBoundary)) {
		gameState = GameState::lost;
	}
}
