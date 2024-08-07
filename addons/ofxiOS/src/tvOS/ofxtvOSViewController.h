//
//  ofxtvOSViewController.h
//  tvOS+OFLib
//
//  Created by Daniel Rosser on 26/10/2015.

#pragma once
#include "ofxiOSConstants.h"
#if defined(TARGET_OF_IOS) && defined(TARGET_OF_TVOS) && !defined(TARGET_OF_WATCHOS) && !defined(TARGET_OF_XROS)
#ifndef ofxtvOSViewController_h
#define ofxtvOSViewController_h

#import <UIKit/UIKit.h>

class ofxiOSApp;
@class ofxiOSEAGLView;

@interface ofxtvOSViewController : UIViewController

@property (nonatomic, retain) ofxiOSEAGLView * glView;

- (instancetype)initWithFrame:(CGRect)frame app:(ofxiOSApp *)app;
- (instancetype)initWithFrame:(CGRect)frame app:(ofxiOSApp *)app sharegroup:(EAGLSharegroup *)sharegroup;

@end

#endif /* ofxtvOSViewController_h */
#endif
