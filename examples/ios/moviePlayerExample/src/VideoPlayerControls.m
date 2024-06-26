//
//  VideoPlayerControls.m
//  moviePlayerExample
//
//  Created by lukasz karluk on 21/05/12.
//

#import "VideoPlayerControls.h"

static NSString * kPlayButtonLabelText = @"PLAY";
static NSString * kPauseButtonLabelText = @"PAUSE";
static NSString * kLoadButtonLabelText = @"LOAD";
static NSString * kUnloadButtonLabelText = @"UNLOAD";
static NSString * kNativeOnButtonLabelText = @"NATIVE ON";
static NSString * kNativeOffButtonLabelText = @"NATIVE OFF";
static NSString * kLoopOnButtonLabelText = @"LOOP ON";
static NSString * kLoopOffButtonLabelText = @"LOOP OFF";
static NSString * kMuteOnButtonLabelText = @"MUTE ON";
static NSString * kMuteOffButtonLabelText = @"MUTE OFF";

@interface VideoPlayerControls () {
	IBOutlet UIButton * playPauseButton;
	IBOutlet UIButton * loadButton;
	IBOutlet UIButton * loopButton;
	IBOutlet UIButton * nativeButton;
	IBOutlet UIButton * muteButton;
	IBOutlet UISlider * slider;
	IBOutlet UILabel * timeLabel;
	IBOutlet UILabel * topLabel;
	IBOutlet UILabel * bottomLabel;
	IBOutlet UILabel * newFrameLabel;
	CGRect controlsFrameShow;
	CGRect controlsFrameHide;
	BOOL bShow;
}

@end

@implementation VideoPlayerControls

- (void)dealloc {

}

- (void)viewDidLoad {
	[super viewDidLoad];
    app = (ofApp*)ofGetAppPtr();
	[self setLoad:YES];
	[self setPlay:YES];
	[self setNative:NO];
	[self setLoop:NO];
    delegate = [[VideoPlayerControlsDelegateForOF alloc] initWithApp:app];
	CGRect screenRect = [UIScreen mainScreen].bounds;
	self.view.frame = screenRect;
}

- (void)viewDidUnload {
	[super viewDidUnload];
}

//----------------------------------------------------- external setters.
- (void)setPlay:(BOOL)bPlay {
	if(bPlay) {
		[playPauseButton setTitle:kPauseButtonLabelText forState:UIControlStateNormal];
	} else {
		[playPauseButton setTitle:kPlayButtonLabelText forState:UIControlStateNormal];
	}
}

- (void)setNative:(BOOL)bNative {
	if(bNative) {
		[nativeButton setTitle:kNativeOnButtonLabelText forState:UIControlStateNormal];
		bottomLabel.text = @"native video";
	} else {
		[nativeButton setTitle:kNativeOffButtonLabelText forState:UIControlStateNormal];
		bottomLabel.text = @"video pixels";
	}
}

- (void)setLoop:(BOOL)bLoop {
	if(bLoop) {
		[loopButton setTitle:kLoopOnButtonLabelText forState:UIControlStateNormal];
	} else {
		[loopButton setTitle:kLoopOffButtonLabelText forState:UIControlStateNormal];
	}
}

- (void)setMute:(BOOL)bMute {
	if(bMute) {
		[muteButton setTitle:kMuteOnButtonLabelText forState:UIControlStateNormal];
	} else {
		[muteButton setTitle:kMuteOffButtonLabelText forState:UIControlStateNormal];
	}
}

- (void)setLoad:(BOOL)bLoad {
	if(bLoad) {
		[loadButton setTitle:kUnloadButtonLabelText forState:UIControlStateNormal];
		
		[self setPlay:YES];
		[self setNative:NO];
		[self setLoop:NO];
		
	} else {
		[loadButton setTitle:kLoadButtonLabelText forState:UIControlStateNormal];
	}
	
	playPauseButton.enabled = bLoad;
	loopButton.enabled = bLoad;
	nativeButton.enabled = bLoad;
	muteButton.enabled = bLoad;
	slider.enabled = bLoad;

	if(!bLoad) {
		[self setPosition:0];
		[self setTimeInSeconds:0];
	}
}

- (void)setNewFrame:(BOOL)bNewFrame {
	if(bNewFrame) {
		newFrameLabel.text = @"newFrame = YES";
	} else {
		newFrameLabel.text = @"newFrame = NO";
	}
}

- (void)setPosition:(float)position {
	if (isfinite(position)) {
		[slider setValue:position];
	} else {
		NSLog(@"[VideoPlayerControls setPosition:] received an invalid value.");
	}
}

- (void)setTimeInSeconds:(NSTimeInterval)seconds {
	if (isfinite(seconds)) {
		if(seconds < 0.0) {
			seconds = 0.0;
		}
		int secondsInt = round(seconds);
		int minutes = secondsInt / 60;
		secondsInt -= minutes * 60;
		int secondsOnes = secondsInt % 10;
		int secondsTens = secondsInt / 10;
		
		timeLabel.text = [NSString stringWithFormat:@"%i:%i%i", minutes, secondsTens, secondsOnes];
	}
}

//----------------------------------------------------- ui handlers.
- (IBAction)playPauseButtonPressed:(id)sender {
	UIButton * button = (UIButton *)sender;
	
	if([button.titleLabel.text isEqualToString:kPauseButtonLabelText]) {
        [self->delegate pausePressed];
	} else if([button.titleLabel.text isEqualToString:kPlayButtonLabelText]) {
        [self->delegate playPressed];
	}
}

- (IBAction)scrubBegin:(id)sender {
	[self->delegate scrubBegin];
}

- (IBAction)scrub:(id)sender {
	float position = slider.value;
    [self->delegate scrubToPosition:position];
}

- (IBAction)scrubEnd:(id)sender {
	[self->delegate scrubEnd];
}

- (IBAction)loadButtonPressed:(id)sender {
	UIButton * button = (UIButton *)sender;
	
	if([button.titleLabel.text isEqualToString:kLoadButtonLabelText]) {
        [self->delegate loadPressed];
	} else if([button.titleLabel.text isEqualToString:kUnloadButtonLabelText]) {
        [self->delegate unloadPressed];
	}
}

- (IBAction)loopButtonPressed:(id)sender {
	UIButton * button = (UIButton *)sender;
	
	if([button.titleLabel.text isEqualToString:kLoopOffButtonLabelText]) {
		[self->delegate loopOnPressed];
	} else if([button.titleLabel.text isEqualToString:kLoopOnButtonLabelText]) {
		[self->delegate loopOffPressed];
	}
}

- (IBAction)nativeButtonPressed:(id)sender {
	UIButton * button = (UIButton *)sender;
    
	if([button.titleLabel.text isEqualToString:kNativeOffButtonLabelText]) {
		[self->delegate nativeOnPressed];
	} else if([button.titleLabel.text isEqualToString:kNativeOnButtonLabelText]) {
		[self->delegate nativeOffPressed];
	}
}

- (IBAction)muteButtonPressed:(id)sender {
	UIButton * button = (UIButton *)sender;
	
	if([button.titleLabel.text isEqualToString:kMuteOffButtonLabelText]) {
        [self->delegate muteOnPressed];
	} else if([button.titleLabel.text isEqualToString:kMuteOnButtonLabelText]) {
        [self->delegate muteOffPressed];
	}
}

@end
