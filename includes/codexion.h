/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 11:11:33 by opernod           #+#    #+#             */
/*   Updated: 2026/05/01 13:21:41 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# include "stdlib.h"
# include <pthread.h>
# include <stdio.h>
# include <sys/time.h>
# include <unistd.h>
# include <limits.h>
# include <string.h>

typedef enum e_schedule
{
	FIFO,
	EDF
}				t_schedule;

typedef struct s_args
{
	int			nb_coders;
	int			time_burnout;
	int			time_compile;
	int			time_debug;
	int			time_refactor;
	int			nb_compiles_required;
	int			dongle_cooldown;
	t_schedule	scheduler;
}				t_args;

typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;
typedef struct s_data	t_data;

typedef struct s_data
{
	pthread_t			monitor_thread;
	t_args				args;
	t_dongle			*all_dongles;
	t_coder				*all_coders;
	long				time_at_start;
	int					simulation_running;
	pthread_mutex_t		sim_mutex;
	pthread_mutex_t		print_mutex;
	int					started_threads;
}						t_data;

typedef struct s_coder
{
	pthread_t			thread;
	pthread_mutex_t		mutex;
	t_data				*data;
	t_dongle			*first_dongle;
	t_dongle			*second_dongle;
	long				last_compile_time;
	int					nb_compiles;
	int					id;
}						t_coder;

typedef struct s_dongle
{
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	t_coder				*coders[2];
	long				last_used;
	int					is_currently_used;
	int					nb_coders_waiting;

}						t_dongle;

long					ft_atol(const char *str);
int						init_args(char **argv, t_args *args);
int						init_data(t_data *data);
void					print_status(t_coder *coder, char *msg, int force);
void					join_threads(t_data *data, int count);
int						start_threads(t_data *data);
void					try_add_to_queue(t_dongle *dongle, t_coder *coder);
int						is_cooldown_active(t_dongle *dongle, t_args args);
void					pop_queue(t_dongle *dongle);
int						take_dongles(t_coder *coder, t_args args);
void					release_dongles(t_coder *coder);
long					get_time_in_ms(void);
void					ft_usleep(long time_in_ms, t_data *data);
void					*monitor_routine(void *arg);
void					lock_two_mutexes(t_coder *mutex1, t_coder *mutex2);
#endif