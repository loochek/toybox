#include <stdio.h>

#include "lerror.hpp"
#include "article.hpp"

const char *beginning = "\\documentclass{article} \
\\usepackage[utf8]{inputenc} \
\\usepackage{breqn} \
\\begin{document} \
\\begin{titlepage} \
\\begin{center} \
\\Large \
\\textbf{Moscow institute of physics and technology} \
proudly presents \
\\vfill \
\\LARGE \
High-precision, moon-phase independent calculation of the limit of the increment of the function value divided by the increment of the argument \
\\vfill \
Moscow\\\\ \
\\vspace{0.2cm} \
2077 \
\\end{center} \
\\end{titlepage} \
\\section{Relevance of the work} \
Nowadays, the task of finding a derivative has become much more complicated. Due to the development of technical progress, fewer and fewer people want to be educated, since their CCPs are already capable of solving all their problems more complicated than adding 2 and 2. They prefer a riotous life to this. Someone is simply wasting money in glamorous establishments, and someone is trying to make money with criminals in order to also be able to visit glamorous establishments. A small group of MIPT scientists came together to join forces and write a program that responds to blah blah blah why are u reading this lol. \
\\section{Science work}";

const char *ending = "\\section{Data sources} \
\\begin{itemize} \
\\item \
(Yes, we still using protocols from previous century) \\\\ \
https://github.com/loochek/homework/tree/master/industrial/differentiator \
\\item \
Algebra 10-11 by Mordkovich, 2009 \
\\item \
Mert ýigitler gaýrat üçin dogulýar by Gurbanguly Berdimuhamedov \
\\item Holy Bible by Jesus, p. 666 \
\\end{itemize} \
\\end{document};";

void make_article(expr_node_t *tree_root, char var, node_pool_t *pool)
{
    LERR_RESET();
    EXPR_CHECK_RET(tree_root,);
    
    FILE *file = fopen("article.tex", "w");

    fprintf(file, "%s", beginning);
    expr_node_t *diff_tree = expr_diff_output(tree_root, var, pool, file);
    if (diff_tree == NULL)
    {
        fclose(file);
        return;
    }

    fprintf(file, "%s", ending);
    fclose(file);

    expr_destroy(diff_tree, pool);

    system("pdflatex article.tex");
}