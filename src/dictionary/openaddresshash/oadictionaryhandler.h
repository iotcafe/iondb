/******************************************************************************/
/**
@file		oadictionaryhandler.h
@author		Scott Ronald Fazackerley
@brief		The handler for a hash table using linear probing.
*/
/******************************************************************************/

#ifndef OADICTIONARYHANDLER_H_
#define OADICTIONARYHANDLER_H_

#include "./../dicttypes.h"
#include "./../dictionary.h"
#include "./../../kv_system.h"
#include "oahash.h"

/**
@brief Struct used to for instance of a given dictionary.
 */
typedef struct oa_dictionary
{
	//what needs to go in here?
	char 		*dictionary_name;	/**<The name of the dictionary*/
	hashmap_t 	*hashmap;			/**<The map that the operations
	 	 	 	 	 	 	 	 		will operate upon*/
} oa_dictionary_t;

/**
 @brief Cursor for dictionary specific implementations
 @todo What happens to the cursor if the collection is modified during traversal?
 */
typedef struct oadict_cursor
{
	hash_t				first;		/**<First visited spot*/
	hash_t				current;	/**<Currently visited spot*/
	char				status;		/**@todo what is this for again as there are two status */
} oadict_cursor_t;


/*
typedef struct equality_cursor
{
	dict_cursor_t 	super;
		*< Cursor supertype this type inherits from.

}equality_cursor_t;
*/


/**
@brief		Dictionary cursor for equality queries.
@details	Used when a dictionary supports multiple vvalues for a given key.

			This subtype should be extended when supported for a given
			dictionary.
*/
typedef struct oadict_equality_cursor
{
	dict_cursor_t			super;			/**<Super type this cursor inherits from*/
	oadict_cursor_t			cursor_info;	/**<Super type to dict implementation*/
	ion_key_t				value;
	boolean_t				(* equal)(dictionary_t *, ion_key_t *);
										/**< A pointer to an equality function. */
} oadict_equality_cursor_t;
/*

*
@brief		Dictionary cursor for range queries.
@details	This subtype should be extended when supported
			for a given dictionary.

typedef struct range_cursor
{
	dict_cursor_t	super;
		*< Cursor supertype this type inherits from.
	boolean_t		(* range)(dictionary_t *, ion_key_t *, ion_key_t *);
		*< A pointer to a range function.
} range_t;

*
@brief		Dictionary cursor for equality queries.
@details	Used when a user gives a function pointer to evaluate
			over each record in the dictionary.

			This subtype should be extended when supported for a given
			dictionary.

typedef struct predicate_cursor
{
	dict_cursor_t	super;
		*< Cursor supertype this type inherits from.
	boolean_t		(* predicate)(dictionary_t *, void *);			// TODO FIXME the void * needs to be dealt with
		*< A pointer to function that that filters records.
} predicate_cursor_t;
*/


/**
@brief		Registers a specific handler for a  dictionary instance.

@details	Registers functions for handlers.  This only needs to be called
			once for each type of dictionary that is present.

@param 		handler
				The handler for the dictionary instance that is to be
				initialized.
@return		The status of the initialization.
 */
err_t
oadict_init(
	dictionary_handler_t 	*handler
);

/**
@brief		Inserts a @p key and @p value into the dictionary.

@param 		dictionary
				The dictionary instance to insert the value into.
@param 		key
				The key to use.
@param 		value
				The value to use.
@return		The status on the insertion of the record.
 */
err_t
oadict_insert(
	dictionary_t 	*dictionary,
	ion_key_t 		*key,
	ion_value_t 	*value
);

/**
@brief 		Queries a dictionary instance for the given @p key and returns
			the associated @p value.

@details	Queries a dictionary instance for the given @p key and returns
			the associated @p value.  If the @p write_concern is set to
			wc_insert_unique then if the @key exists already, an error will
			be generated as duplicate keys are prevented.  If the
			@p write_concern is set to wc_update, the updates are allowed.
			In this case, if the @p key exists in the hashmap, the @p value
			will be updated.  If the @p key does not exist, then a new item
			will be inserted to hashmap.

@param 		dictionary
				The instance of the dictionary to query.
@param 		key
				The key to search for.
@param 		value
				A pointer that is used to return the value associated with
				the provided key.  The function will malloc memory for the
				value and it is up to the consumer the free the associated
				memory.
@return		The status of the query.
 */
err_t
oadict_query(
	dictionary_t 	*dictionary,
	ion_key_t 		key,
	ion_value_t		*value
);

/**
 @brief		Creates an instance of a dictionary.

 @details	Creates as instance of a dictionary given a @p key_size and
 	 	 	 @p value_size, in bytes as well as the @p dictionary size
 	 	 	 which is the number of buckets available in the hashmap.

 @param 	key_size
 	 			The size of the key in bytes.
 @param 	value_size
				The size of the value in bytes.
 @param 	dictionary_size
 	 	 	 	 The size of the hashmap in discrete units
 @param 	handler
 	 	 	 	 THe handler for the specific dictionary being created.
 @param 	dictionary
 	 	 	 	 The pointer declared by the caller that will reference
 	 	 	 	 the instance of the dictionary created.
 @return	The status of the creation of the dictionary.
 */
err_t
oadict_create_dictionary(
		int 					key_size,
		int 					value_size,
		int 					dictionary_size,
		dictionary_handler_t 	*handler,
		dictionary_t 			*dictionary
);

/**
@brief		Deletes the @p key and assoicated value from the dictionary
			instance.

@param 		dictionary
				The instance of the dictionary to delete from.
@param 		key
				The key that is to be deleted.
@return		The status of the deletion
 */
err_t
oadict_delete(
		dictionary_t 	*dictionary,
		ion_key_t 		*key
);

/**
@brief 		Deletes an instance of the dictionary and associated data.

@param 		dictionary
				The instance of the dictionary to delete.
@return		The status of the dictionary deletion.
 */
err_t
oadict_delete_dictionary(
		dictionary_t 	*dictionary
);

/**
@brief		Updates the value for a given key.

@details	Updates the value for a given @pkey.  If the key does not currently
			exist in the hashmap, it will be created and the value sorted.

@param 		dictionary
				The instance of the dictionary to be updated.
@param 		key
				The key that is to be updated.
@param 		value
				The value that is to be updated.
@return		The status of the update.
 */
err_t
oadict_update(
		dictionary_t 	*dictionary,
		ion_key_t 		*key,
		ion_value_t 	*value
);

/**
@brief 		Finds multiple instances of a keys that satisfy the provided
 	 	 	 predicate in the dictionary.

@details 	Generates a cursor that allows the traversal of items where
			the items key satisfies the @p predicate (if the underlying
			implementation allows it).

@param 		dictionary
				The instance of the dictionary to search.
@param 		predicate
				The predicate to be used as the condition for matching.
@param 		cursor
				The pointer to a cursor which is caller declared but callee
				is responsible for populating.
@return		The status of the operation.
 */
err_t
oadict_find(
		dictionary_t 	*dictionary,
		predicate_t 	*predicate,
		dict_cursor_t 	*cursor
);

/**
@brief		Next function to query and retrieve the next
			<K,V> that stratifies the predicate of the cursor.

@param 		cursor
				The cursor to iterate over the results.
@return		The status of the cursor.
 */
err_t
oadict_next(
	dict_cursor_t 	*cursor,
	ion_value_t		value
);

#endif /* OADICTIONARYHANDLER_H_ */
