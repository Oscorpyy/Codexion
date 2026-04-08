/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 13:09:44 by opernod           #+#    #+#             */
/*   Updated: 2026/04/08 16:12:49 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct s_args
{
	int	number_of_coders;
	int	time_to_burnout;
	int	time_to_compile;
	int	time_to_debug;
	int	time_to_refactor;
	int	number_of_compiles_required;
	int	dongle_cooldown;
	char	*scheduler;
}					t_args;

typedef struct s_coder {
	int			id;
	pthread_t	thread_id;

} t_coder;

typedef enum e_dongle {
	DONGLE_AVAILABLE,
	DONGLE_IN_USE,
	DONGLE_COOLDOWN
} t_dongle;

typedef struct s_all {
	t_dongle	dongle;
	t_coder 	coder;
	t_args		*args;
} t_all;

int		parssing(t_args *args, int argc, char **argv);