/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:59:33 by opernod           #+#    #+#             */
/*   Updated: 2026/04/08 16:15:10 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void *supervisor(void *arg)
{
	t_coder *coder = (t_coder *)arg;

	printf("Codeur %d : thread créé avec succès\n", coder->id);
	return (NULL);
}

int init_threads(pthread_mutex_t mutex, t_coder *coders, int number_of_coders)
{
	int i = 0;

	while (i < number_of_coders) {
		coders[i].id = i + 1;
		if (pthread_create(&coders[i].thread_id, NULL, supervisor, &coders[i]) != 0) {
			printf("Erreur : impossible de créer le thread du codeur %d\n", coders[i].id);
			return (1);
		}
		i++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_args	*args;
	t_coder *coder;
	pthread_mutex_t mutex;
	t_all all;

	all = malloc(sizeof(t_all));
	args = malloc(sizeof(t_args));
	if (!args || !all)
	{
		printf("Error: malloc failed\n");
		return (1);
	}
	if (parssing(args, argc, argv))
	{
		free(args);
		printf("Error: parssing failed\n");
		return (1);
	}
	coder = malloc(sizeof(t_coder) * args->number_of_coders);
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

	printf("parsing done\n");
	printf("number_of_coders: %d\n", args->number_of_coders);
	printf("time_to_burnout: %d\n", args->time_to_burnout);
	printf("time_to_compile: %d\n", args->time_to_compile);
	printf("time_to_debug: %d\n", args->time_to_debug);
	printf("time_to_refactor: %d\n", args->time_to_refactor);
	printf("number_of_compiles_required: %d\n",
		args->number_of_compiles_required);
	printf("dongle_cooldown: %d\n", args->dongle_cooldown);
	printf("scheduler: %s\n", args->scheduler);
	init_threads(mutex, coder, args->number_of_coders);
	free(args);
	return (0);
}
