/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 13:09:44 by opernod           #+#    #+#             */
/*   Updated: 2026/05/13 15:27:38 by opernod          ###   ########lyon.fr   */
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

typedef struct s_order
{
	int	first;
	int	second;
	int	*f_flag;
	int	*s_flag;
}	t_order;

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
	pthread_mutex_t	*dongle_mutexes;
	struct s_coder	*coders;
	t_args			*args;
	int				is_running;
	long			start_time;
	pthread_mutex_t	run_mutex;
	long			*dongle_cooldown_end;
	pthread_mutex_t	cooldown_mutex;
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
	pthread_mutex_t	*left_dongle;
	pthread_mutex_t	*right_dongle;
	int				has_left;
	int				has_right;
}	t_coder;

int		check_running(t_all *all);
int		parssing(t_args *args, int argc, char **argv);
int		compile_cycle(t_all *a, t_coder *c, int l, int r);
int		take_dongles_edf(t_all *a, t_coder *c, int l, int r);
int		check_burnout(t_all *all, t_coder *co, int i, long current);
int		setup_mutex(pthread_mutex_t *m, t_all *all, t_args *args, t_coder *c);
void	*coder_routine(void *arg);
void	print_state(t_coder *coder, char *state, int force);
void	ft_usleep(long time_in_ms, t_coder *coder);
void	release_dongle(t_all *all, int idx, int *flag);
void	set_order(t_coder *c, int l, int r, t_order *o);
void	acquire_dongles_edf(t_all *all, t_coder *coder);
void	wait_for_dongle(t_all *all, int idx, int *flag);
void	acquire_dongles_fifo(t_all *all, t_coder *coder);
void	free_all(t_args *args, t_all *all, t_coder *coder);
void	cleanup(t_args *args, t_all *a, t_coder *co, pthread_mutex_t *mut);
void	mutex_lock_ordered(pthread_mutex_t *m1, pthread_mutex_t *m2,
			int id1, int id2);
long	get_time(void);
t_coder	*get_opponent(t_all *a, t_coder *c, int t_d);


#endif
