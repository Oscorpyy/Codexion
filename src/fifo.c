/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 10:00:00 by opernod           #+#    #+#             */
/*   Updated: 2026/04/24 14:09:04 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

long	get_time(void);
int		check_running(t_all *all);
void	print_state(t_coder *coder, char *state);

void	acquire_dongles_fifo(t_all *all, t_coder *coder)
{
	while (check_running(all))
	{
		print_state(coder, "has taken a dongle");
		print_state(coder, "has taken a dongle");
		print_state(coder, "is compiling");
		pthread_mutex_lock(&coder->coder_mutex);
		coder->last_compile_time = get_time();
		pthread_mutex_unlock(&coder->coder_mutex);
		ft_usleep(all->args->time_to_compile, all);
		pthread_mutex_lock(&coder->coder_mutex);
		coder->compiles_done++;
		pthread_mutex_unlock(&coder->coder_mutex);
		if (all->args->number_of_compiles_required != -1
			&& coder->compiles_done >= all->args->number_of_compiles_required)
			break ;
		print_state(coder, "is debugging");
		ft_usleep(all->args->time_to_debug, all);
		print_state(coder, "is refactoring");
		ft_usleep(all->args->time_to_refactor, all);
	}
}
