/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_edf_util.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 16:07:23 by opernod           #+#    #+#             */
/*   Updated: 2026/05/01 13:18:16 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	lock_two_mutexes(t_coder *mutex1, t_coder *mutex2)
{
	pthread_mutex_lock(&mutex1->mutex);
	pthread_mutex_lock(&mutex2->mutex);
}
