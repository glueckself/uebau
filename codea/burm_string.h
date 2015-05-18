














const char* burm_string[] = {
"",
"; fun: OP_CALL(expr)\n",

"; expr: numexpr \n",
"; expr: listexpr\n",
"; expr: OP_HEAD(listexpr) \n",
"; expr: OP_TAIL(listexpr) \n",
"; expr: OP_IDENT\n",

";listexpr: OP_DOT(expr, expr)\n",
";listexpr: OP_DOT(expr, listexpr)\n",
";listexpr: OP_DOT(expr, numeexpr)\n",
";listexpr: OP_DOT(numexpr, expr) \n",
";listexpr: OP_DOT(numexpr, listexpr) \n",
";listexpr: OP_DOT(numexpr, numexpr)\n",
";listexpr: OP_DOT(listexpr,expr) \n",
";listexpr: OP_DOT(listexpr,numexpr)\n",
";listexpr: OP_DOT(listexpr,listexpr) \n",
//";listexpr: OP_IDENT\n",
"; listexpr: expr\n",

"; numexpr: OP_ISNUM(expr) \n",
"; numexpr: OP_ISNUM(numexpr)\n",
"; numexpr: OP_ISNUM(imm)\n",
"; numexpr: OP_ISLIST(expr)\n",
"; numexpr: OP_ISLIST(numexpr) \n",
"; numexpr: OP_ISLIST(imm) \n",
"; numexpr: OP_ISFUN(expr) \n",
"; numexpr: OP_ISFUN(numexpr)\n",
"; numexpr: OP_ISFUN(imm)\n",
"; numexpr: OP_PLUS(numexpr, numexpr)\n",
"; numexpr: OP_PLUS(numexpr, imm)\n",
"; numexpr: OP_PLUS(imm, numexpr)\n",
"; numexpr: OP_MINUS(numexpr, numexpr) \n",
"; numexpr: OP_MINUS(numexpr, imm) \n",
"; numexpr: OP_MINUS(imm, numexpr) \n",
"; numexpr: OP_MULT(numexpr, numexpr)\n",
"; numexpr: OP_MULT(numexpr, imm)\n",
"; numexpr: OP_MULT(imm, numexpr)\n",
"; numexpr: OP_AND(numexpr, numexpr) \n",
"; numexpr: OP_AND(numexpr, imm) \n",
"; numexpr: OP_AND(imm, numexpr) \n",
"; numexpr: OP_NOT(numexpr)\n",
"; numexpr: OP_NOT(imm)\n",
"; numexpr: OP_LESS(numexpr, numexpr)\n",
"; numexpr: OP_EQUAL(expr, expr) \n",
//"; numexpr: OP_NUM \n",
"; numexpr: OP_IDENT \n",
"; numexpr: expr \n",
"; numexpr: imm \n",

"; imm: OP_PLUS(imm, imm) \n",
"; imm: OP_MINUS(imm, imm)\n",
"; imm: OP_MULT(imm, imm) \n",
"; imm: OP_NOT(imm) \n",
"; imm: OP_AND(imm, imm)\n",
"; imm: OP_LESS(imm, imm) \n",
"; imm: OP_EQUAL(imm, imm)\n",
"; imm: OP_ISNUM(imm) \n",
"; imm: OP_ISFUN(imm) \n",
"; imm: OP_ISLIST(imm)\n",
"; imm: OP_NUM\n",
};