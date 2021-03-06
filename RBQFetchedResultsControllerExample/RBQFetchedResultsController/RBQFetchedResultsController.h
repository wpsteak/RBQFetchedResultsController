//
//  RBQFetchedResultsController.h
//  RBQFetchedResultsControllerTest
//
//  Created by Lauren Smith on 1/2/15.
//  Copyright (c) 2015 Roobiq. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RBQFetchRequest.h"
#import "RBQSafeRealmObject.h"

@import CoreData;

@class RBQFetchedResultsController;

#pragma mark - RBQFetchedResultsSectionInfo

/**
 *  This class is used by the RBQFetchedResultsController to pass along section info.
 */
@interface RBQFetchedResultsSectionInfo : NSObject

/**
 *  The number of objects in the section.
 */
@property (nonatomic, readonly) NSUInteger numberOfObjects;

/**
 *  The objects in the section (generated on-demand and not thread-safe).
 */
@property (nonatomic, readonly) RLMResults *objects;

/**
 *  The name of the section.
 */
@property (nonatomic, readonly) NSString *name;

@end

#pragma mark - RBQFetchedResultsControllerDelegate

/**
 *  Delegate to pass along the changes identified by the RBQFetchedResultsController.
 */
@protocol RBQFetchedResultsControllerDelegate <NSObject>

@optional

/**
 *  Indicates that the controller has started identifying changes.
 *
 *  @param controller controller instance that noticed the change on its fetched objects
 */
- (void)controllerWillChangeContent:(RBQFetchedResultsController *)controller;

/**
 Notifies the delegate that a fetched object has been changed due to an add, remove, move, or
 update. Enables RBQFetchedResultsController change tracking.
 
 Changes are reported with the following heuristics:
 
  * On add and remove operations, only the added/removed object is reported. It’s assumed that all
    objects that come after the affected object are also moved, but these moves are not reported.
 
  * A move is reported when the changed attribute on the object is one of the sort descriptors used
    in the fetch request. An update of the object is assumed in this case, but no separate update
    message is sent to the delegate.
 
  * An update is reported when an object’s state changes, but the changed attributes aren’t part of
   the sort keys.
 
 @param controller controller instance that noticed the change on its fetched objects
 @param anObject changed object represented as a RBQSafeRealmObject for thread safety
 @param indexPath indexPath of changed object (nil for inserts)
 @param type indicates if the change was an insert, delete, move, or update
 @param newIndexPath the destination path for inserted or moved objects, nil otherwise
 */

- (void)controller:(RBQFetchedResultsController *)controller
   didChangeObject:(RBQSafeRealmObject *)anObject
       atIndexPath:(NSIndexPath *)indexPath
     forChangeType:(NSFetchedResultsChangeType)type
      newIndexPath:(NSIndexPath *)newIndexPath;

/**
 *  The fetched results controller reports changes to its section before changes to the fetched result
 objects.
 *
 *  @param controller   controller controller instance that noticed the change on its fetched objects
 *  @param section      changed section represented as a RBQFetchedResultsSectionInfo object
 *  @param sectionIndex the section index of the changed section
 *  @param type         indicates if the change was an insert or delete
 */
- (void)controller:(RBQFetchedResultsController *)controller
  didChangeSection:(RBQFetchedResultsSectionInfo *)section
           atIndex:(NSUInteger)sectionIndex
     forChangeType:(NSFetchedResultsChangeType)type;

/**
 *  This method is called at the end of processing changes by the controller
 *
 *  @param controller controller instance that noticed the change on its fetched objects
 */
- (void)controllerDidChangeContent:(RBQFetchedResultsController *)controller;

@end

#pragma mark - RBQFetchedResultsController

/**
 *  The class is used to monitor changes from a RBQRealmNotificationManager to convert these changes into specific index path or section index changes. Typically this is used to back a UITableView and support animations when items are inserted, deleted, or changed.
 */
@interface RBQFetchedResultsController : NSObject

/**
 *  The fetch request for the controller
 */
@property (nonatomic, readonly) RBQFetchRequest *fetchRequest;

/**
 *  The section name key path used to create the sections. Can be nil if no sections.
 */
@property (nonatomic, readonly) NSString *sectionNameKeyPath;

/**
 *  The delegate to pass the index path and section changes to.
 */
@property (nonatomic, weak) id <RBQFetchedResultsControllerDelegate> delegate;

/**
 *  The name of the cache used internally to represent the tableview structure.
 */
@property (nonatomic, readonly) NSString *cacheName;

/**
 *  All the objects that match the fetch request.
 */
@property (nonatomic, readonly) RLMResults *fetchedObjects;

/**
 *  Deletes the cached section information with the given name
 
    @warning This method should only be called if there are no strong references to the FRC that was using the cache. If deleting all caches (by passing nil for name), it is recommended to do this in didFinishLaunchingWithOptions: in AppDelegate.
 *
 *  @param name The name of the cache file to delete. If name is nil, deletes all cache files.
 */
+ (void)deleteCacheWithName:(NSString *)name;

/**
 *  Constructor method to initialize the controller
 
    @warning *Important:* Specify a cache name if deletion of the cache later on is necessary
 *
 *  @param fetchRequest       the RBQFetchRequest for the controller
 *  @param sectionNameKeyPath the section name key path used to create sections (can be nil)
 *  @param name               the cache name (if nil, cache will be built using an in-Memory Realm and not persisted)
 *
 *  @return A new instance of RBQFetchedResultsController
 */
- (id)initWithFetchRequest:(RBQFetchRequest *)fetchRequest
        sectionNameKeyPath:(NSString *)sectionNameKeyPath
                 cacheName:(NSString *)name;

/**
 *  Constructor method to initialize the controller
 
    @warning This constructor is primarily for use in testing the FRC. If you don't want to persist the cache, then it is recommended to use nil for the cache name which internally will create an in-Memory Realm for you.
 *
 *  @param fetchRequest       the RBQFetchRequest for the controller
 *  @param sectionNameKeyPath the section name key path used to create sections (can be nil)
 *  @param inMemoryRealm      the in-memory Realm to be used for the internal cache
 *
 *  @return A new instance of RBQFetchedResultsController
 */
- (id)initWithFetchRequest:(RBQFetchRequest *)fetchRequest
        sectionNameKeyPath:(NSString *)sectionNameKeyPath
        inMemoryRealmCache:(RLMRealm *)inMemoryRealm;

/**
 *  Method to tell the controller to perform the fetch
 *
 *  @return Indicates if the fetch was successful
 */
- (BOOL)performFetch;

/**
 *  Call this method to force the cache to be rebuilt. 
 
    A potential use case would be to call this in a @catch after trying to call endUpdates for the table view. If an exception is thrown, then the cache will be rebuilt and you can call reloadData on the table view.
 */
- (void)reset;

/**
 *  Method to retrieve the number of rows for a given section index
 *
 *  @param index section index
 *
 *  @return number of rows in the section
 */
- (NSInteger)numberOfRowsForSectionIndex:(NSInteger)index;

/**
 *  Method to retrieve the number of sections represented by the fetch request
 *
 *  @return number of sections
 */
- (NSInteger)numberOfSections;

/**
 *  Method to retrieve the title for a given section index
 *
 *  @param section section index
 *
 *  @return The title of the section
 */
- (NSString *)titleForHeaderInSection:(NSInteger)section;

/**
 *  Retrieve the RBQSafeRealmObject for a given index path
 *
 *  @param indexPath the index path of the object
 *
 *  @return RBQSafeRealmObject
 */
- (RBQSafeRealmObject *)safeObjectAtIndexPath:(NSIndexPath *)indexPath;

/**
 *  Retrieve the RLMObject for a given index path
 
    @warning *Important:* Returned object is not thread-safe.
 *
 *  @param indexPath the index path of the object
 *
 *  @return RLMObject
 */
- (id)objectAtIndexPath:(NSIndexPath *)indexPath;

/**
 *  Retrieve the RLMObject in a specific Realm for a given index path
 *
 *  @param realm     the Realm in which the RLMObject is persisted
 *  @param indexPath the index path of the object
 *
 *  @return RLMObject
 */
- (id)objectInRealm:(RLMRealm *)realm
        atIndexPath:(NSIndexPath *)indexPath;

/**
 *  Retrieve the index path for a safe object in the fetch request
 *
 *  @param safeObject RBQSafeRealmObject
 *
 *  @return index path of the object
 */
- (NSIndexPath *)indexPathForSafeObject:(RBQSafeRealmObject *)safeObject;

/**
 *  Retrieve the index path for a RLMObject in the fetch request
 *
 *  @param object RLMObject
 *
 *  @return index path of the object
 */
- (NSIndexPath *)indexPathForObject:(RLMObject *)object;

@end
