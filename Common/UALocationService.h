/*
 Copyright 2009-2012 Urban Airship Inc. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 2. Redistributions in binaryform must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided withthe distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE URBAN AIRSHIP INC``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 EVENT SHALL URBAN AIRSHIP INC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <CoreLocation/CoreLocation.h>
#import <Availability.h>
#import "UALocationProviderDelegate.h"
#import "UALocationEvent.h"

@class UALocationService;
/** The UALocationServiceDelegate receives 
 location updates from any of the UALocationServices
 */
@protocol UALocationServiceDelegate <NSObject>

///---------------------------------------------------------------------------------------
/// @name UALocationServiceDelegate
///---------------------------------------------------------------------------------------

@optional
/** Updates the delegate when the location service generates an error
 @param service Location service that generated the error
 @param error Error passed from a CLLocationManager
 */
- (void)locationService:(UALocationService*)service didFailWithError:(NSError*)error;

/** Updates the delegate when authorization status has changed
 @warning *Important:* Available on iOS 4.2 or greater only
 @param service Location service reporting the change
 @param status  The updated location authorization status
 */
- (void)locationService:(UALocationService*)service didChangeAuthorizationStatus:(CLAuthorizationStatus)status;

/** Delegate callbacks for updated locations only occur while the app is in the foreground. If you need background location updates
 create a separate CLLocationManager
 @warning *Important:* In the background, this method is called and given a limited amount of time to operate, including the time
 necessary to update UrbanAirship. Extensive work done by the method while backgrounded could result in location data not being
 recorded or sent
 @param service The service reporting the location update
 @param newLocation The updated location reported by the service
 @param oldLocation The previously reported location
 */
- (void)locationService:(UALocationService*)service didUpdateToLocation:(CLLocation*)newLocation fromLocation:(CLLocation*)oldLocation;
@end

@class UAStandardLocationProvider;
@class UASignificantChangeProvider;

/** The UALocationService class provides an interface to both the location services on 
 device and the Urban Airship API. 
 */
@interface UALocationService : NSObject <UALocationProviderDelegate> {
    
    NSTimeInterval minimumTimeBetweenForegroundUpdates_;
    CLLocation *lastReportedLocation_;
    NSDate *dateOfLastLocation_;
    id <UALocationServiceDelegate> delegate_;
    BOOL promptUserForLocationServices_;
    BOOL automaticLocationOnForegroundEnabled_;
    BOOL backroundLocationServiceEnabled_;
}

///---------------------------------------------------------------------------------------
/// @name Standard Location Accuracy and Distance
///---------------------------------------------------------------------------------------

/** The distance filter on the standard location provider
 @returns CLLocationDistance The current distance filter on the standard location provider
 */
- (CLLocationDistance)standardLocationDistanceFilter;
/** Sets the distance filter on the standard location provider. 
 @param distanceFilter The new distance filter.
 */
- (void)setStandardLocationDistanceFilter:(CLLocationDistance)distanceFilter;
/** The desired accuracy on the standard location provider
 @return CLLocationAccuracy The current desired accuracy
 */
- (CLLocationAccuracy)standardLocationDesiredAccuracy;
/** Sets the standard location desired accuracy
 @param desiredAccuracy The new desired accuracy
 */
- (void)setStandardLocationDesiredAccuracy:(CLLocationAccuracy)desiredAccuracy;

///---------------------------------------------------------------------------------------
/// @name Location Services Authorization
///---------------------------------------------------------------------------------------

/** Current setting allowing UA location services
 @return YES UA location services are allowed, and will start if authorized and enabled
 @return NO UA location services are not allowed, and will not start, even if authorized and enabled
 */
+ (BOOL)airshipLocationServiceEnabled;

/** The allows UA Location Services to report location
 
 @param airshipLocationServiceEnabled If set to YES, all UA location services will run
 if the system reports that location services are available and authorized. This setting will not
 override the users choice to disable location services and is safe to enable when user preferences
 have not been establishd. If NO, UA Location Services are disabled. This setting is persited in 
 NSUserDefaults
 */
+ (void)setAirshipLocationServiceEnabled:(BOOL)airshipLocationServiceEnabled;

/** Reports the current authorization status of location services as reported by the 
 system. This refers to the global location service setting switch. 
 
 @return YES if location services are reported as enabled by the system.
 @return NO if location services are reported as not enabled by the system.
 */
+ (BOOL)locationServicesEnabled;

/** Reports the current authorization status of location services as reported by the
 system. 
 @warning For iOS < 4.2, this value is updated after an attempt has been made to start location
 services, and is persisted from that point on. Prompting the user is the only way to set this value.
 
 @return YES if the user has authorized location services, or has yet to be asked about location services.
 @return NO if the user has explictly disabled location services
 */
+ (BOOL)locationServiceAuthorized;

/** This method checks the underlying Core Location service for to see if a user
 will receive a prompt requesting permissino for Core Location services to run.
 
 @warning On iOS < 4.2 This method's default value is YES until after an intial attempt to start location services 
 has been made by UALocationService. If the user declines location services, that value will be persited, and future attempts
 to start location services will require that promptUserForLocationServices be set to YES
 
 @return NO If Core Location services are enabled and the user has explicity authorized location services
 @return YES If ANY of the following are true
    - Location services are not enabled (The global locaiton service setting in Settings is disabled)
    - Location services are explicitly not authorized (The per app location service setting in Settings is disabled)
    - The user has not been asked yet to allow location services. (Location servies are enabled, and CLLocationManager reports kCLAuthorizationStatusNotDetermined)
 */
+ (BOOL)coreLocationWillPromptUserForPermissionToRun;

/** This flag will allows UA Location Services to re prompt the user to allow services
 The user may have explicitly disallowed location services, so reprompting them may not 
 be welcome. A value of NO (default value) will ensure that the user is only prompted in the
 case where system location services have indicated that the user has not disabled location 
 services and has not been previously prompted for location services.
 
 @return YES An attempt to start location services will be made even if this results in prompting
 the user to allow location services.
 @return NO Location services will not start if the user has previously disabled location services. 
 */
@property (nonatomic, assign) BOOL promptUserForLocationServices;

///--------------------------------------------------------------------------------------
/// @name Recent Activity
///---------------------------------------------------------------------------------------

/** The most recently received location available from the CLLocationManager objec. This may be more accurate than
 the last reported location, and it may also be nil. See CLLocationManager documetation for more details. 
 @return The most recent location, if one is available
 @return nil if no recent location is available
 */
- (CLLocation*)location;

/// Last location reported to Urban Airship 
@property (nonatomic, retain,readonly) CLLocation *lastReportedLocation;

/// Date of last location event reported 
@property (nonatomic, retain, readonly) NSDate *dateOfLastLocation;

/// UALocationServiceDelage for location service callbacks
@property (nonatomic, assign) id <UALocationServiceDelegate> delegate;

///---------------------------------------------------------------------------------------
/// @name Automatic Location Services 
///---------------------------------------------------------------------------------------

/// Starts the GPS (Standard Location) and acquires a single location on every launch
@property (nonatomic, assign) BOOL automaticLocationOnForegroundEnabled;

/// Allows location services to continue in the background 
@property (nonatomic, assign) BOOL backgroundLocationServiceEnabled;

/** Minimum time between automatic updates that are tied to app foreground events.
 Default value is 120 seconds
 */
@property (nonatomic, assign) NSTimeInterval minimumTimeBetweenForegroundUpdates;

///---------------------------------------------------------------------------------------
/// @name Status of Services
///---------------------------------------------------------------------------------------

/// Status for GPS service 
@property (nonatomic, assign, readonly) UALocationProviderStatus standardLocationServiceStatus;

/// Status for NETWORK (cell tower) events
@property (nonatomic, assign, readonly) UALocationProviderStatus significantChangeServiceStatus;

/** Status for single location service */
@property (nonatomic, assign, readonly) UALocationProviderStatus singleLocationServiceStatus;

///---------------------------------------------------------------------------------------
/// @name Purpose 
///---------------------------------------------------------------------------------------

/** Purpose for location services shown to user
 when prompted to allow location services to begin. The default value
 is kUALocationServiceDefaultPurpose listed in UAirship.m. This value is set on
 all new location services.
 */
- (NSString*)purpose;

/** The current purpose 
 @returns An NSString with the current purpose*/
- (void)setPurpose:(NSString*)purpose;

///---------------------------------------------------------------------------------------
/// @name Creating the Location Service
///---------------------------------------------------------------------------------------

/** Returns a UALocationService object with the given purpose. The purpose
 string is passed to the UALocationProviders and set on the CLLocationManager. 
 This is displayed to the user when asking for location authorization.
 @param purpose The description that is displayed to the user when prompted for authorization.
 */
- (id)initWithPurpose:(NSString*)purpose;

/** Starts the Standard Location service and 
 sends location data to Urban Airship. This service will continue updating if the location property is 
 declared in the Info.plist. Please see the Location Awareness Programming guide:
 http://developer.apple.com/library/ios/#documentation/UserExperience/Conceptual/LocationAwarenessPG/Introduction/Introduction.html
 for more information. If the standard location service is not setup for background
 use, it will automatically resume once the app is brought back into the foreground.
 This will not start the location service if the app is not enabled and authorized. To force
 location services to start, set the UALocationServicesAllowed property to YES and call this method.
 This will prompt the user for permission if location services have not been started previously,
 or if the user has purposely disabled location services. Given that location services were probably 
 disabled for a reason, this prompt might not be welcomed. 
 */

///---------------------------------------------------------------------------------------
/// @name Starting and Stopping Location Services
///---------------------------------------------------------------------------------------

/** Start the standard location service */
- (void)startReportingStandardLocation;

/** Stops the standard location service */
- (void)stopReportingStandardLocation;

/** Starts the Significant Change location service
 and sends location data to Urban Airship. This service 
 will continue in the background if stopMonitoringSignificantLocationChanges
 is not called before the app enters the background.
 This will not start the location service if the app is not enabled and authorized. To force
 location services to start, set the UALocationServicesAllowed property to YES and call this method.
 This will prompt the user for permission if location services have not been started previously,
 or if the user has purposely disabled location services. Given that location services were probably 
 disabled for a reason, this prompt might not be welcomed.
 **/
- (void)startReportingSignificantLocationChanges;

/** Stops the Significant Change location service */
- (void)stopReportingSignificantLocationChanges;

///---------------------------------------------------------------------------------------
/// @name Analytics
///---------------------------------------------------------------------------------------

/** Creates a UALocationEvent and enques it with the Analytics service
 @param location The location to be sent to the Urban Airship analytics service
 @param provider The provider that generated the location. Data is pulled from the provider for analytics
*/ 
 - (void)reportLocationToAnalytics:(CLLocation*)location fromProvider:(id<UALocationProviderProtocol>)provider;

/** Starts the standard location service long enough to obtain a location an then uploads
 it to Urban Airship.
*/
- (void)reportCurrentLocation;

/** Sends a location directly to Urban Airship. The required parameters are taken from the CLLocation and 
 the CLLocationManager. The UALocationEventUpdateType is helpful in providing the end developer with information
 regarding the use of location in app. The possible values are:
 
 - UALocationEventUpdateTypeChange This is one of the periodic services, intended for the significant change or region monitoring service
 - UALocationEventUpdateTypeContinuous This is meant for the standard location service.
 - UALocationEventUpdateTypeSingle This is meant for a one time service, like the reportCurrentLocation method on this class
 @param location A CLLocation 
 @param locationManager The location manager that provided the location
 @param updateTypeOrNil The update type as described above or nil. 
 */
- (void)reportLocation:(CLLocation*)location 
 fromLocationManager:(CLLocationManager*)locationManager 
      withUpdateType:(UALocationEventUpdateType*)updateTypeOrNil;


@end
