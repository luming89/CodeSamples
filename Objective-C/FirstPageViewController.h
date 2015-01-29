//
//  FirstPageViewController.h
//
//  Created by Luming on 12-21-2014.
//  Copyright (c) 2014 Luming. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface FirstPageViewController : UIViewController
{
    IBOutlet UIView *pageFirst;
    IBOutlet UIView *pageSecond;
    IBOutlet UIView *pageThird;
    int time; 
    BOOL tend;
    CGRect frame1;
    CGRect frame2;
    CGRect frame3;
}


-(IBAction)clickPrev:(id)sender;

@end
