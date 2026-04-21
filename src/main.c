/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:59:33 by opernod           #+#    #+#             */
/*   Updated: 2026/04/21 16:59:40 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"
#include <sys/time.h>
#include <unistd.h>

long get_time(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int check_running(t_all *all)
{
	int status;

	pthread_mutex_lock(&all->run_mutex);
	status = all->is_running;
	pthread_mutex_unlock(&all->run_mutex);
	return (status);
}

void print_state(t_coder *coder, char *state)
{
	long current_time;

	pthread_mutex_lock(coder->write_mutex);
	if (check_running(coder->all))
	{
		current_time = get_time() - coder->all->start_time;
		printf("%ld %d %s\n", current_time, coder->id, state);
	}
	pthread_mutex_unlock(coder->write_mutex);
}

void *coder_routine(void *arg)
{
	t_coder *coder = (t_coder *)arg;

	if (strcmp(coder->all->args->scheduler, "fifo") == 0)
		acquire_dongles_fifo(coder->all, coder);
	else
		acquire_dongles_edf(coder->all, coder);
	
	return (NULL);
}

int init_threads(pthread_mutex_t *mutex, t_coder *coders, t_all *all, int number_of_coders)
{
	int i = 0;

	all->start_time = get_time();
	while (i < number_of_coders) {
		coders[i].id = i + 1;
		coders[i].write_mutex = mutex;
		coders[i].all = all;
		coders[i].last_compile_time = all->start_time;
		coders[i].compiles_done = 0;
		
		pthread_mutex_init(&coders[i].coder_mutex, NULL);
		
		if (pthread_create(&coders[i].thread_id, NULL, coder_routine, &coders[i]) != 0) {
			pthread_mutex_lock(mutex);
			printf("Erreur : impossible de créer le thread du codeur %d\n", coders[i].id);
			pthread_mutex_unlock(mutex);
			return (i);
		}
		i++;
	}
	return (i);
}

void monitor_routine(t_all *all, t_coder *coders)
{
	int i;
	int all_compiled;
	long current_time;

	while (check_running(all))
	{
		i = 0;
		all_compiled = 1;
		while (i < all->args->number_of_coders)
		{
			pthread_mutex_lock(&coders[i].coder_mutex);
			current_time = get_time();
			if (current_time - coders[i].last_compile_time > all->args->time_to_burnout)
			{
				pthread_mutex_lock(&all->run_mutex);
				all->is_running = 0;
				pthread_mutex_unlock(&all->run_mutex);
				
				pthread_mutex_lock(coders[i].write_mutex);
				printf("%ld %d burned out\n", current_time - all->start_time, coders[i].id);
				pthread_mutex_unlock(coders[i].write_mutex);
				
				pthread_mutex_unlock(&coders[i].coder_mutex);
				return;
			}
			if (all->args->number_of_compiles_required == -1 || coders[i].compiles_done < all->args->number_of_compiles_required)
				all_compiled = 0;
			pthread_mutex_unlock(&coders[i].coder_mutex);
			i++;
		}
		if (all->args->number_of_compiles_required != -1 && all_compiled)
		{
			pthread_mutex_lock(&all->run_mutex);
			all->is_running = 0;
			pthread_mutex_unlock(&all->run_mutex);
			return;
		}
		usleep(1000);
	}
}

int	main(int argc, char **argv)
{
	t_args	*args;
	t_coder *coder;
	pthread_mutex_t mutex;
	t_all *all;
	int i;
	int num_created;

	all = calloc(1, sizeof(t_all));
	args = calloc(1, sizeof(t_args));
	if (!args || !all)
	{
		if (args)
			free(args);
		if (all)
			free(all);
		printf("Error: malloc failed\n");
		return (1);
	}
	if (parssing(args, argc, argv))
	{
		free(args);
		free(all);
		printf("Error: parssing failed\n");
		return (1);
	}
	if (args->number_of_coders <= 0 || args->number_of_compiles_required == 0)
	{
		free(args);
		free(all);
		return (0);
	}
	coder = calloc(1, sizeof(t_coder) * args->number_of_coders);
	if (!coder)	{
		printf("Error: malloc failed\n");
		free(args);
		return (1);
	}
	if (pthread_mutex_init(&mutex, NULL) != 0)
	{
		printf("Error: mutex initialization failed\n");
		free(args);
		free(coder);
		return (1);
	}

	all->args = args;
	all->is_running = 1;
	pthread_mutex_init(&all->run_mutex, NULL);
	all->coders = coder;

	num_created = init_threads(&mutex, coder, all, args->number_of_coders);
	
	if (num_created == args->number_of_coders)
		monitor_routine(all, coder);
	else
	{
		pthread_mutex_lock(&all->run_mutex);
		all->is_running = 0;
		pthread_mutex_unlock(&all->run_mutex);
	}
	
	i = 0;
	while (i < num_created) {
		pthread_join(coder[i].thread_id, NULL);
		pthread_mutex_destroy(&coder[i].coder_mutex);
		i++;
	}

	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&all->run_mutex);
	free(args);
	free(coder);
	free(all);
	return (0);
}
