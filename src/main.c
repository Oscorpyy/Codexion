/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 13:11:14 by opernod           #+#    #+#             */
/*   Updated: 2026/05/01 13:18:06 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	print_error(char *msg)
{
	fprintf(stderr, "%s", msg);
	return (1);
}

static void	clean_data(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->args.nb_coders)
	{
		pthread_mutex_destroy(&data->all_dongles[i].mutex);
		pthread_cond_destroy(&data->all_dongles[i].cond);
		pthread_mutex_destroy(&data->all_coders[i].mutex);
		i++;
	}
	free(data->all_dongles);
	free(data->all_coders);
}

int	main(int argc, char **argv)
{
	t_data	data;

	if (argc != 9 || !init_args(argv, &data.args) || !init_data(&data))
	{
		return (print_error("Error\n"));
	}
	if (data.args.nb_compiles_required == 0)
	{
		clean_data(&data);
		return (0);
	}
	if (!start_threads(&data))
	{
		clean_data(&data);
		return (print_error("Error starting threads\n"));
	}
	join_threads(&data, data.args.nb_coders);
	clean_data(&data);
	return (0);
}
