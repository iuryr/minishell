/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Juliany Bernardo <julberna@student.42sp    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/14 19:35:43 by Juliany Ber       #+#    #+#             */
/*   Updated: 2024/02/20 18:04:23 by Juliany Ber      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int		check_ll(char *nbr);
char	*trim_prefix(const char *s1, const char *set);

void	ft_exit(t_token *tk, t_ast *ast, t_hash **ht, int last_exit)
{
	long int	exit_code;

	exit_code = last_exit;
	ft_putendl_fd(tk->literal, 1);
	if ((tk->next
			&& (!ft_isdigit(*tk->next->literal) && *tk->next->literal != '-'))
		|| (tk->next && check_ll(tk->next->literal)))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(tk->next->literal, 2);
		ft_putendl_fd(": numeric argument required", 2);
		exit_code = 2;
	}
	else if (tk->next && tk->next->next)
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		return ;
	}
	else if (tk->next)
		exit_code = ft_atol(tk->next->literal);
	free_ht(ht);
	finisher(tk, ast);
	exit(exit_code);
}

int	check_ll(char *exit_code)
{
	int			ret;
	char		*n;
	char		*nn;
	const char	*max_ll = "9223372036854775807";
	const char	*min_ll = "-9223372036854775808";

	ret = LIE;
	if (exit_code[0] == '-')
	{
		n = trim_prefix(&exit_code[1], "0");
		nn = ft_calloc(ft_strlen(n) + 2, sizeof(char));
		ft_strlcat(nn, "-", ft_strlen(n) + 2);
		ft_strlcat(nn, n, ft_strlen(n) + 2);
		if (ft_strlen(nn) > ft_strlen(min_ll) || ft_strcmp(min_ll, nn) < 0)
			ret = TRUTH;
		free(nn);
	}
	else
	{
		n = trim_prefix(exit_code, "0");
		if (ft_strlen(n) > ft_strlen(max_ll) || ft_strcmp(max_ll, n) < 0)
			ret = TRUTH;
	}
	free(n);
	return (ret);
}

char	*trim_prefix(const char *s1, const char *set)
{
	size_t	size;
	char	*trimmed;

	if (s1 == NULL || set == NULL)
		return (NULL);
	while (ft_strchr(set, *s1) && *s1)
		s1++;
	size = ft_strlen(s1);
	trimmed = ft_calloc(size + 1, sizeof(char));
	if (!trimmed)
		return (NULL);
	ft_strlcpy(trimmed, s1, size + 1);
	return (trimmed);
}
