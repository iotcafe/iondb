/*
 * idEncoder.c
 *
 *  Created on: Jul 22, 2015
 *      Author: workstation
 */

#include "fileEncoder.h"

void
destory(
	filename_t						*file
)
{
	if (file->parent.parent_filename != NULL)
	{
		free(file->parent.parent_filename);
		file->parent.parent_filename = NULL;
	}
}

err_t
encode_parent_id(
	filename_t						*file
)
{

	/** parent file name is 00000XXX.YYY
	 * 							 	 ^^^-----encoded file type
	 * 							 ^^^---------encoded parent id
	 */

	file->destroy = destory;

	if ((file->instance_id > 4095) || (file->parent.type > 4095))
	{
		file->parent.parent_filename = NULL;
		return err_illegal_state;
	}

	if (0 == (file->parent.parent_filename 		= (char*)malloc(FILENAME_SIZE)))
	{
		file->child.child_filename = NULL;
		return err_illegal_state;
	}

	int idx = 0;

	ion_dictionary_id_t	id			= file->instance_id;
	datastore_e file_type			= file->parent.type;

	for (; idx < (8 - MAX_PARENT_NIBBLES);idx++)
	{
		file->parent.parent_filename[idx] = '0';
	}

	for (; idx < 8; idx++)
	{
		file->parent.parent_filename[idx] = encode[(((unsigned char)id) & 0x0F)];		/** extract value */
		id = (id >> NUMBER_OF_BITS);					/** shift down */
	}

	file->parent.parent_filename[idx++] = '.';

	for (; idx < 12; idx++)
	{
		file->parent.parent_filename[idx] = parent_type[(((unsigned char)file_type) & 0x0F)];	/** extract value */
		file_type = (file_type >> NUMBER_OF_BITS);					/** shift down */
	}

	file->parent.parent_filename[idx] = 0;

	return err_ok;

}

err_t
encode_child_id(
	filename_t						*file
)
{

	file->destroy = destory;

	if (file->instance_id > 4095)
	{
		file->child.child_filename = NULL;
		return err_illegal_state;
	}
	/** parent file name is 00000XXX.YYY
	 * 							 	 ^^^-----encoded file type
	 * 							 ^^^---------encoded parent id
	 */

	if (0 == (file->child.child_filename 		= (char*)malloc(FILENAME_SIZE)))
	{
		file->child.child_filename = NULL;
		return err_illegal_state;
	}

	ion_dictionary_id_t	id			= file->instance_id;
		ion_dictionary_id_t	child_id	= file->child.child_id;
	int idx = 0;

	for (; idx < 8; idx++)
	{
		file->child.child_filename[idx] = encode[(((unsigned char)child_id) & 0x0F)];		/** extract value */
		child_id = (child_id >> NUMBER_OF_BITS);					/** shift down */
	}

	file->child.child_filename[idx++] = '.';

	for (; idx < 12; idx++)
	{
		file->child.child_filename[idx] = encode[(((unsigned char)id) & 0x0F)];	/** extract value */
		id = (id >> NUMBER_OF_BITS);								/** shift down */
	}

	file->child.child_filename[idx] = 0;

	return err_ok;

}