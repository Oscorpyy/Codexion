/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 13:09:44 by opernod           #+#    #+#             */
/*   Updated: 2026/04/24 17:54:43 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_args
{
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	char	*scheduler;
}	t_args;

typedef enum e_dongle
{
	DONGLE_AVAILABLE,
	DONGLE_IN_USE,
	DONGLE_COOLDOWN
}	t_dongle;

typedef struct s_all
{
	t_dongle		*dongles;
	struct s_coder	*coders;
	t_args			*args;
	int				is_running;
	long			start_time;
	pthread_mutex_t	run_mutex;
}	t_all;

typedef struct s_coder
{
	int				id;
	pthread_t		thread_id;
	pthread_mutex_t	*write_mutex;
	t_all			*all;
	long			last_compile_time;
	int				compiles_done;
	pthread_mutex_t	coder_mutex;
}	t_coder;

int		parssing(t_args *args, int argc, char **argv);
void	acquire_dongles_fifo(t_all *all, t_coder *coder);
void	acquire_dongles_edf(t_all *all, t_coder *coder);
void	cleanup(t_args *args, t_all *a, t_coder *co, pthread_mutex_t *mut);
long	get_time(void);
int		check_running(t_all *all);
void	free_all(t_args *args, t_all *all, t_coder *coder);
int		setup_mutex(pthread_mutex_t *m, t_all *all, t_args *args, t_coder *c);
void	ft_usleep(long time_in_ms, t_coder *coder);
int		check_burnout(t_all *all, t_coder *co, int i, long current);

#endif
