/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Juliany Bernardo <julberna@student.42sp    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 12:05:13 by iusantos          #+#    #+#             */
/*   Updated: 2024/02/22 18:49:31 by Juliany Ber      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

void	executor(t_meta *meta)
{
	if (meta->ast->right == NULL)
		run_simple_command(meta->ast->left, meta);
	else
		run_pipeline(meta->ast, 0, meta);
}

void	run_pipeline(t_ast *ast, int in_fd, t_meta *meta)
{
	int		pipe_fd[2];
	pid_t	child_pid;

	if (pipe(pipe_fd) == -1)
		return ;
	if (ast->right->type == CMD)
	{
		child_pid = fork();
		if (child_pid == 0)
			exec_left(ast->left->data, in_fd, pipe_fd, meta);
		child_pid = fork();
		if (child_pid == 0)
			exec_right(ast->right->data, pipe_fd, meta);
	}
	else
	{
		child_pid = fork();
		if (child_pid == 0)
			exec_left(ast->left->data, in_fd, pipe_fd, meta);
		close(pipe_fd[1]);
		run_pipeline(ast->right, pipe_fd[0], meta);
		close(pipe_fd[0]);
	}
	close(in_fd);
	close(pipe_fd[1]);
	close(pipe_fd[0]);
	while (cap_n_upd_exit_status(meta) != -1)
		;
	return ;
}

void	run_executable(t_cmd *data, t_meta *meta)
{
	int		exec_return;
	char	**array_of_strings;

	array_of_strings = NULL;
	array_of_strings = stringfy(data->word_list);
	exec_return = execve(data->pathname, array_of_strings, NULL);
	if (exec_return == -1)
	{
		perror(strerror(errno));
		free_array_of_strings(array_of_strings, get_size(data->word_list));
		free(array_of_strings);
		finisher(*meta);
		free_ht(meta->hash);
		exit(errno);
	}
}

void	free_array_of_strings(char **array, int size)
{
	int	index;

	if (array == NULL)
		return ;
	index = 0;
	while (index <= size)
	{
		free(array[index]);
		index++;
	}
}

char	**stringfy(t_word *wl)
{
	char	**array;
	int		size;
	int		index;

	size = get_size(wl) + 1;
	array = ft_calloc(size, sizeof(char *));
	index = 0;
	while (wl->next != NULL)
	{
		array[index] = ft_strdup(wl->word);
		index++;
		wl = wl->next;
	}
	array[index] = ft_strdup(wl->word);
	return (array);
}

int	get_size(t_word *wl)
{
	int	nelem;

	nelem = 1;
	while (wl->next != NULL)
	{
		nelem++;
		wl = wl->next;
	}
	return (nelem);
}

void	run_builtin(t_meta *meta, t_word *wl)
{
	int				i;
	char			*exit_str;
	const t_builtin	builtin_dict[] = {{"cd", cd}, {"echo", echo}, \
			{"env", env}, {"exit", ft_exit}, {"export", export}, \
			{"pwd", pwd}, {"unset", unset}};

	i = -1;
	exit_str = NULL;
	while (++i < 8)
	{
		if (ft_strcmp(builtin_dict[i].cmd_name, wl->word) == 0)
			exit_str = ft_itoa(builtin_dict[i].function(meta, wl));
	}
	if (!exit_str)
		handle_null_pathname(meta);
	else
		add_or_upd_ht_entry("?", exit_str, meta->hash);
	free(exit_str);
}

void	close_all_fds(void)
{
	int	i;

	i = 0;
	while (i < 1024)
	{
		close(i);
		i++;
	}
}
