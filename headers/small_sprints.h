/***************** Header *********************/
/*
 * University of Southern Denmark
 * Embedded Programming (EMP)
 *
 * MODULENAME: small_sprints.h
 * PROJECT   : Elevator project
 * DESCRIPTION: Implements a better version of snprintf to save on ram
 */

#ifndef SMALL_SPRINTS_H
#define SMALL_SPRINTS_H


int snprintf( char *buf, unsigned int count, const char *format, ... );


#endif /* SMALL_SPRINTS_H */
