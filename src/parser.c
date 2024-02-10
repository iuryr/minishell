/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julberna <julberna@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/17 17:55:58 by iusantos          #+#    #+#             */
/*   Updated: 2024/02/10 18:33:18 by julberna         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

void	parser(t_token *tokens, t_ast_node **ast)
{
	*ast = parse_pipeline(&tokens, NULL);
}

t_ast_node	*parse_pipeline(t_token **tokens, t_ast_node *parent)
{
	t_ast_node	*pl_node;

	set_pl(&pl_node, &parent, tokens);
	if (!pl_node)
		return (NULL);
	pl_node->left = parse_cmd(tokens, pl_node);
	if (pl_node->left == NULL)
		return (NULL);
	if (*tokens == NULL)
		return (pl_node);
	else if (!ft_strncmp((*tokens)->literal, "|", 1) && has_other_pipes(*tokens)
		&& ft_strncmp((*tokens)->next->literal, "|", 1))
	{
		*tokens = (*tokens)->next;
		pl_node->right = parse_pipeline(tokens, pl_node);
	}
	else if (!ft_strncmp((*tokens)->literal, "|", 1)
		&& !has_other_pipes(*tokens) && (*tokens)->next != NULL)
	{
		*tokens = (*tokens)->next;
		pl_node->right = parse_cmd(tokens, pl_node);
	}
	else
		ft_printf("Syntax error near token %s\n", (*tokens)->literal);
	return (pl_node);
}

t_ast_node	*parse_cmd(t_token **tokens, t_ast_node *parent)
{
	t_ast_node	*cmd_node;

	set_cmd(&cmd_node, &parent);
	while (*tokens != NULL && ft_strncmp((*tokens)->literal, "|", 1))
	{
		if ((*tokens)->type == REDIRECT && (*tokens)->next->type == WORD)
		{
			append_redirect(new_redirect(*tokens), cmd_node->data->redirects);
			*tokens = (*tokens)->next->next;
		}
		else if ((*tokens)->type == WORD)
		{
			append_wle(new_wle((*tokens)->literal), cmd_node->data->word_list);
			*tokens = (*tokens)->next;
		}
		else
		{
			ft_printf("Syntax error near token %s\n", (*tokens)->literal);
			free_data(cmd_node->data);
			free(cmd_node);
			return (NULL);
		}
	}
	return (cmd_node);
}
