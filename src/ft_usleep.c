/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_usleep.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 12:45:00 by opernod           #+#    #+#             */
/*   Updated: 2026/04/29 16:20:09 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

void	ft_usleep(long time_in_ms, t_coder *coder)
{
	long	start;

	start = get_time();
	while ((get_time() - start) < time_in_ms && check_running(coder->all))
	{
		pthread_mutex_lock(&coder->coder_mutex);
		if (check_burnout(coder->all, coder->all->coders,
				coder->id - 1, get_time()))
			break ;
		pthread_mutex_unlock(&coder->coder_mutex);
		usleep(50);
	}
}
