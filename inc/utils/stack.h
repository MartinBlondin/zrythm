/*
 * Copyright (C) 2019 Alexandros Theodotou <alex at zrythm dot org>
 *
 * This file is part of Zrythm
 *
 * Zrythm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zrythm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Zrythm.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * \file
 *
 * Stack implementation.
 */

#ifndef __UTILS_STACK_H__
#define __UTILS_STACK_H__

#include <stdlib.h>

#include <gtk/gtk.h>

/**
 * @addtogroup utils
 *
 * @{
 */

#define STACK_PUSH(s, element) \
  stack_push (s, (void *) element)

typedef struct Stack
{
  void **           elements;
  int               max_length;
  volatile gint     top;
} Stack;

Stack *
stack_new (int length);

int
stack_size (Stack * s);

int
stack_is_empty (Stack * s);

int
stack_is_full (Stack * s);

void *
stack_peek (Stack * s);

void *
stack_peek_last (Stack * s);

void
stack_push (Stack *    s,
            void *     element);

void *
stack_pop (Stack * s);

/**
 * Pops the last element and moves everything back.
 */
void *
stack_pop_last (Stack * s);

/**
 * @}
 */

#endif
