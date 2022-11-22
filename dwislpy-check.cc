#include <variant>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "dwislpy-check.hh"
#include "dwislpy-ast.hh"
#include "dwislpy-util.hh"

bool is_int(Type type) {
    return std::holds_alternative<IntTy>(type);
}

bool is_str(Type type) {
    return std::holds_alternative<StrTy>(type);
}

bool is_bool(Type type) {
    return std::holds_alternative<BoolTy>(type);
}

bool is_None(Type type) {
    return std::holds_alternative<NoneTy>(type);
}

bool operator==(Type type1, Type type2) {
    if (is_int(type1) && is_int(type2)) {
        return true;
    }
    if (is_str(type1) && is_str(type2)) {
        return true;
    }
    if (is_bool(type1) && is_bool(type2)) {
        return true;
    }
    if (is_None(type1) && is_None(type2)) {
        return true;
    }
    return false;
}

bool operator!=(Type type1, Type type2) {
    return !(type1 == type2);
}

Rtns plus(Locn lo, Rtns ret1, Rtns ret2) {
    if (std::holds_alternative<Void>(ret1)) {
        if (std::holds_alternative<Void>(ret2)) {
            return Rtns {Void {}};
        } else if (std::holds_alternative<VoidOr>(ret2)) {
            return ret2;
        } else {
            Type t2 = std::get<Type>(ret2);
            return Rtns { VoidOr { t2 }};
        }
    } else if (std::holds_alternative<VoidOr>(ret1)) {
        if (std::holds_alternative<Void>(ret2)) {
            return ret1;
        } else if (std::holds_alternative<VoidOr>(ret2)) {
            Type t1 = std::get<VoidOr>(ret1).type;
            Type t2 = std::get<VoidOr>(ret2).type;
            if (t1 == t2) {
                return ret1; // does not matter which
            } else {
                throw DwislpyError(lo, "Return type behavior mismatch in Retn addition statement.");
            }
        } else {
            Type t1 = std::get<VoidOr>(ret1).type;
            Type t2 = std::get<Type>(ret2);
            if (t1 == t2) {
                return ret1; // does not matter which
            } else {
                throw DwislpyError(lo, "Return type behavior mismatch in Retn addition statement.");
            }
        }
    } else { // if_rt holds Type
        if (std::holds_alternative<Void>(ret2)) {
            Type t1 = std::get<Type>(ret1);
            return Rtns {VoidOr {t1}};
        } else if (std::holds_alternative<VoidOr>(ret2)) {
            Type t1 = std::get<Type>(ret1);
            Type t2 = std::get<VoidOr>(ret2).type;
            if (t1 == t2) {
                return ret1; // does not matter which
            } else {
                throw DwislpyError(lo, "Return type behavior mismatch in Retn addition statement.");
            }
        } else {
            Type t1 = std::get<Type>(ret1);
            Type t2 = std::get<Type>(ret2);
            if (t1 == t2) {
                return ret1; // does not matter which
            } else {
                throw DwislpyError(lo, "Return type behavior mismatch in Retn addition statement.");
            }
        }
    }
}

std::string type_name(Type type) {
    if (is_int(type)) {
        return "int";
    }
    if (is_str(type)) {
        return "str";
    }
    if (is_bool(type)) {
        return "bool";
    } 
    if (is_None(type)) {
        return "None";
    }
    return "wtf";
}


Type type_of(Rtns rtns) {
    if (std::holds_alternative<VoidOr>(rtns)) {
        return std::get<VoidOr>(rtns).type;
    }
    if (std::holds_alternative<Type>(rtns)) {
        return std::get<Type>(rtns);
    }
    return Type {NoneTy {}}; // Should not happen.
}


unsigned int Defn::arity(void) const {
    return symt.get_frmls_size();
}

Type Defn::returns(void) const {
    return rety;
}

SymInfo_ptr Defn::formal(int i) const {
    return symt.get_frml(i);
}

void Prgm::chck(void) {
    for (std::pair<Name,Defn_ptr> dfpr : defs) {
        dfpr.second->chck(defs);
    }
    Rtns rtns = main->chck(Rtns{Void {}},defs,main_symt);
    if (!std::holds_alternative<Void>(rtns)) {
        DwislpyError(main->where(), "Main script should not return."); // ???
    }
}


void Defn::chck(Defs& defs) {
    Rtns rtns = blck->chck(Rtns{Type{rety}}, defs, symt);
    if (std::holds_alternative<Void>(rtns)) {
        throw DwislpyError(blck->where(), "Definition body never returns."); // ???
    }
    if (std::holds_alternative<VoidOr>(rtns)) {
        throw DwislpyError(blck->where(), "Definition body might not return."); // ???
    }
}

Rtns Blck::chck(Rtns expd, Defs& defs, SymT& symt) {

    Rtns retn = Rtns { Void {}};

    // Scan through the statements and check their return behavior.
    for (Stmt_ptr stmt : stmts) {

        // Check this statement.
        Rtns stmt_rtns = stmt->chck(expd, defs, symt);

        /*
        if (stmt != stmts.back()) {
            if (std::holds_alternative<Type>(stmt_rtns)) {
                throw DwislpyError(where(), "Statements are present after definite return in block.");
            } 
        } else { // last statement
            if (std::holds_alternative<Void>(expd)) {
                if (!std::holds_alternative<Void>(stmt_rtns)) {
                    throw DwislpyError(where(), "Type mismatch for final statement in blck.");
                }
                return Rtns { Void {}};
            } (std::holds_alternative<VoidOr>(expd)) {
                if (std::holds_alternative<Type>(stmt_rtns)) {
                    Type stmt_t = std::get<Type>(stmt_retns); 
                    Type expd_t = std::get<VoidOr>(expd).type;
                    if (stmt_t != expd_t){
                        throw DwislpyError(where(), "Return behavior mismatch for final statement in blck.");
                    }
                    return Rtns { expd_t}
                } else if (std::holds_alternative<VoidOr>(stmt_rtns)) {
                    Type stmt_t = std::get<VoidOr>(stmt_retns).type; 
                    Type expd_t = std::get<VoidOr>(expd).type;
                    if (stmt_t != expd_t){
                        throw DwislpyError(where(), "Return behavior mismatch for final statement in blck.");
                    }
                    return Rtns { expd_t}
                }
                return Rtns {Void}
            } else {
                if (!std::holds_alternative<Type>(stmt_rtns)) {
                    throw DwislpyError(where(), "Return behavior mismatch for final statement in blck.");
                }
                Type stmt_t = std::get<Type>(stmt_rtns);
                Type expd_t = std::get<Type>(expd);
                if (stmt_t != expd_t) {
                    throw DwislpyError(where(), "Type mismatch for final statement in blck.");
                } 
                return Rtns { expd_t};
            }
        }
        */
        
        // much better logically
        if (std::holds_alternative<Void>(retn)) {
            retn = stmt_rtns;
        } else if (std::holds_alternative<VoidOr>(retn)) {
            if (std::holds_alternative<Type>(stmt_rtns)) {
                retn = stmt_rtns;
            }
        }
    }            

    return retn;
}


Rtns Asgn::chck([[maybe_unused]] Rtns expd, Defs& defs, SymT& symt) {
    if (!symt.has_info(name)) {
        throw DwislpyError(where(), "Variable '" + name + "' never introduced.");
    }
    Type name_ty = symt.get_info(name)->type;
    Type expn_ty = expn->chck(defs,symt);
    if (name_ty != expn_ty) {
        std::string msg = "Type mismatch. Expected expression of type ";
        msg += type_name(name_ty) + ".";
        throw DwislpyError {expn->where(), msg};
    }
    return Rtns {Void {}};
}

Rtns PlEq::chck([[maybe_unused]] Rtns expd, Defs& defs, SymT& symt) {
    if (!symt.has_info(name)) {
        throw DwislpyError(where(), "Variable '" + name + "' never introduced.");
    }
    Type name_ty = symt.get_info(name)->type;
    Type expn_ty = expn->chck(defs,symt);
    if (name_ty != expn_ty) {
        std::string msg = "Type mismatch. Expected expression of type ";
        msg += type_name(name_ty) + ".";
        throw DwislpyError {expn->where(), msg};
    }
    return Rtns {Void {}};
}

Rtns MiEq::chck([[maybe_unused]] Rtns expd, Defs& defs, SymT& symt) {
    if (!symt.has_info(name)) {
        throw DwislpyError(where(), "Variable '" + name + "' never introduced.");
    }
    Type name_ty = symt.get_info(name)->type;
    Type expn_ty = expn->chck(defs,symt);
    if (name_ty != expn_ty) {
        std::string msg = "Type mismatch. Expected expression of type ";
        msg += type_name(name_ty) + ".";
        throw DwislpyError {expn->where(), msg};
    }
    return Rtns {Void {}};
}

Rtns Pass::chck([[maybe_unused]] Rtns expd,
                [[maybe_unused]] Defs& defs,
                [[maybe_unused]] SymT& symt) {
    return Rtns {Void {}};
}


Rtns Prnt::chck([[maybe_unused]] Rtns expd, Defs& defs, SymT& symt) {
    for (Expn_ptr expn : prms) {
        [[maybe_unused]] Type expn_ty = expn->chck(defs,symt);
    }
    return Rtns {Void {}};
}

Rtns Ntro::chck([[maybe_unused]] Rtns expd, Defs& defs, SymT& symt) {

    symt.add_locl(name, type);

    Type name_ty = symt.get_info(name)->type;
    Type expn_ty = expn->chck(defs,symt);
    if (name_ty != expn_ty) {
        std::string msg = "Type mismatch. Expected expression of type ";
        msg += type_name(name_ty) + ".";
        throw DwislpyError {expn->where(), msg};
    }
    return Rtns {Void {}};
}

Rtns Retn::chck(Rtns expd, [[maybe_unused]] Defs& defs, [[maybe_unused]] SymT& symt) {
    Type expn_ty = Type { NoneTy {}};
    if (std::holds_alternative<Type>(expd)) {
        if (expn_ty == type_of(expd)) {
            return Rtns { Type {}};
        } else {
            throw DwislpyError(where(), "Type mismatch for procedure return.");
        }
    }
    if (std::holds_alternative<VoidOr>(expd)) {
        VoidOr v = std::get<VoidOr>(expd);
        if (expn_ty == v.type) {
            return Rtns { Type {}};
        } else {
            throw DwislpyError(where(), "Type mismatch for procedure return.");
        }
    }
        
    throw DwislpyError(where(), "Unexpected return for void procedure.");
}

Rtns RetE::chck(Rtns expd, Defs& defs, SymT& symt) {
    Type expn_ty = expn->chck(defs,symt);
    if (std::holds_alternative<Type>(expd)) {
        if (expn_ty == type_of(expd)) {
            return Rtns { Type {}};
        } else {
            throw DwislpyError(where(), "Type mismatch for function return.");
        }
    }
    if (std::holds_alternative<VoidOr>(expd)) {
        VoidOr v = std::get<VoidOr>(expd);
        if (expn_ty == v.type) {
            return Rtns { Type {}}; // ??? is this VoidOr
        } else {
            throw DwislpyError(where(), "Type mismatch for function return.");
        }
    }
        
    throw DwislpyError(where(), "Unexpected return for void function.");
}


Rtns Proc::chck([[maybe_unused]] Rtns expd, Defs& defs, SymT& symt) {
    if (defs.count(name) == 0) {
        std::string msg = "Type error: procedure '" + name +"'";
        msg += " is not defined.";
        throw DwislpyError { where(), msg };
    }

    Defn_ptr def = defs.at(name);

    if (def->rety != (NoneTy {})) {
        throw DwislpyError { where(), "Error: Function called as procedure." };
    }

    if ((int)def->symt.get_frmls_size() != (int)args.size()) {
        std::string msg = "Incorrect number of args found for function " 
            + name + ": expected " + std::to_string(def->symt.get_frmls_size()) + ", saw " 
            + std::to_string(args.size()) + ".";
        throw DwislpyError { where(), msg };
    }

    for (int i = 0; i < (int)args.size(); ++i) {
        
        Type arg_ty = args[i]->chck(defs,symt);
        // def->!!!
        if (arg_ty != def->symt.get_frml(i)->type) {
            std::string msg = "Type mismatch in argument for function call.";
            throw DwislpyError { where(), msg };
        }
    }
    return Rtns{ Void{}};
}

Rtns Tern::chck(Rtns expd, Defs& defs, SymT& symt) {
    
    [[maybe_unused]] Type expn_ty = expn->chck(defs,symt);
    // should always be able to evaluate to bool !

    Rtns if_rt = if_blck->chck(expd,defs,symt);
    Rtns else_rt = else_blck->chck(expd,defs,symt);

    return plus(where(), if_rt, else_rt);
}

Rtns Whle::chck(Rtns expd, Defs& defs, SymT& symt) {
    
    [[maybe_unused]] Type expn_ty = expn->chck(defs,symt);
    // should always be able to evaluate to bool !

    Rtns blck_rt = blck->chck(expd,defs,symt);
    return plus(where(), blck_rt, Rtns { Void {}});
}

Type Func::chck(Defs& defs, SymT& symt) {

    if (defs.count(name) == 0) {
        std::string msg = "Run-time error: procedure '" + name +"'";
        msg += " is not defined.";
        throw DwislpyError { where(), msg };
    }

    Defn_ptr def = defs.at(name);

    if ((int)def->symt.get_frmls_size() != (int)args.size()) {
        std::string msg = "Incorrect number of args found for function " 
            + name + ": expected " + std::to_string(def->symt.get_frmls_size()) + ", saw " 
            + std::to_string(args.size()) + ".";
        throw DwislpyError { where(), msg };
    }

    for (int i = 0; i < (int)args.size(); ++i) {
        
        Type arg_ty = args[i]->chck(defs,symt);
        // def->!!!
        if (arg_ty != def->symt.get_frml(i)->type) {
            std::string msg = "Type mismatch in argument for function call.";
            throw DwislpyError { where(), msg };
        }
    }
    return def->rety;
}

Type Plus::chck(Defs& defs, SymT& symt) {
    Type left_ty = left->chck(defs,symt);
    Type rght_ty = rght->chck(defs,symt);
    if (is_int(left_ty) && is_int(rght_ty)) {
        return Type {IntTy {}};
    } else if (is_str(left_ty) && is_str(rght_ty)) {
        return Type {StrTy {}};
    } else {
        std::string msg = "Wrong operand types for plus.";
        throw DwislpyError { where(), msg };
    }
}

Type Mnus::chck(Defs& defs, SymT& symt) {
    Type left_ty = left->chck(defs,symt);
    Type rght_ty = rght->chck(defs,symt);
    if (is_int(left_ty) && is_int(rght_ty)) {
        return Type {IntTy {}};
    } else {
        std::string msg = "Wrong operand types for minus.";
        throw DwislpyError { where(), msg };
    }
}

Type Tmes::chck(Defs& defs, SymT& symt) {
    Type left_ty = left->chck(defs,symt);
    Type rght_ty = rght->chck(defs,symt);
    if (is_int(left_ty) && is_int(rght_ty)) {
        return Type {IntTy {}};
    } else {
        std::string msg = "Wrong operand types for times.";
        throw DwislpyError { where(), msg };
    }
}

Type IDiv::chck( Defs& defs, SymT& symt) {
    Type left_ty = left->chck(defs,symt);
    Type rght_ty = rght->chck(defs,symt);
    if (is_int(left_ty) && is_int(rght_ty)) {
        return Type {IntTy {}};
    } else {
        std::string msg = "Wrong operand types for iDiv.";
        throw DwislpyError { where(), msg };
    }
}

Type IMod::chck( Defs& defs, SymT& symt) {
    Type left_ty = left->chck(defs,symt);
    Type rght_ty = rght->chck(defs,symt);
    if (is_int(left_ty) && is_int(rght_ty)) {
        return Type {IntTy {}};
    } else {
        std::string msg = "Wrong operand types for imod.";
        throw DwislpyError { where(), msg };
    }
}

Type Less::chck(Defs& defs, SymT& symt) {
    Type left_ty = left->chck(defs,symt);
    Type rght_ty = rght->chck(defs,symt);
    if (is_int(left_ty) && is_int(rght_ty)) {
        return Type {BoolTy {}};
    } else {
        std::string msg = "Wrong operand types for less.";
        throw DwislpyError { where(), msg };
    }
}

Type LtEq::chck( Defs& defs,  SymT& symt) {
    Type left_ty = left->chck(defs,symt);
    Type rght_ty = rght->chck(defs,symt);
    if (is_int(left_ty) && is_int(rght_ty)) {
        return Type {BoolTy {}};
    } else {
        std::string msg = "Wrong operand types for less than or equal to.";
        throw DwislpyError { where(), msg };
    }
}

Type Eqal::chck(Defs& defs, SymT& symt) {
    // check sub expression
    [[maybe_unused]] Type left_ty = left->chck(defs,symt);
    [[maybe_unused]] Type rght_ty = rght->chck(defs,symt);
    // always a bool
    return Type {BoolTy {}};
}

Type Conj::chck( Defs& defs,  SymT& symt) {
    [[maybe_unused]] Type left_ty = left->chck(defs,symt);
    [[maybe_unused]] Type rght_ty = rght->chck(defs,symt);
    //We want to check the subexpressions, but we don't care about their type for conjunction
    return Type {BoolTy {}}; 
}

Type Disj::chck(Defs& defs, SymT& symt) {
    [[maybe_unused]] Type left_ty = left->chck(defs,symt);
    [[maybe_unused]] Type rght_ty = rght->chck(defs,symt);
    //We want to check the subexpressions, but we don't care about their type for conjunction
    return Type {BoolTy {}}; 
}

Type Negt::chck(Defs& defs, SymT& symt) {
    [[maybe_unused]] Type expn_ty = expn->chck(defs,symt);
    //We want to check the subexpressions, but we don't care about their type for conjunction
    return Type {BoolTy {}}; 
}

Type Ltrl::chck([[maybe_unused]] Defs& defs,[[maybe_unused]]  SymT& symt) {
    if (std::holds_alternative<int>(valu)) {
        return Type {IntTy {}};
    } else if (std::holds_alternative<std::string>(valu)) {
        return Type {StrTy {}};
    } if (std::holds_alternative<bool>(valu)) {
        return Type {BoolTy {}};
    } else {
        return Type {NoneTy {}};
    } 
}

Type Lkup::chck([[maybe_unused]] Defs& defs, [[maybe_unused]] SymT& symt) {
    if (symt.has_info(name)) {
        return symt.get_info(name)->type;
    } else {
        throw DwislpyError {where(), "Unknown identifier."};
    } 
}

Type Inpt::chck(Defs& defs, SymT& symt) {
    // Doesn't check subexpressions.
    // Fix this!!
    Type expn_ty = expn->chck(defs,symt);
    if (is_str(expn_ty)) {
        return Type {StrTy {}}; 
    } else {
        std::string msg = "Wrong expression type for input, expected str.";
        throw DwislpyError { where(), msg };
    }
    
}

Type IntC::chck(Defs& defs, SymT& symt) {
    Type expn_ty = expn->chck(defs,symt);
    if (is_None(expn_ty)) {
        std::string msg = "Cannot convert nonetype to int";
        throw DwislpyError { where(), msg };
    }
    return Type {IntTy {}}; 
}

Type StrC::chck(Defs& defs, SymT& symt) {
    Type expn_ty = expn->chck(defs,symt);
    if (is_None(expn_ty)) {
        std::string msg = "Cannot convert nonetype to str";
        throw DwislpyError { where(), msg };
    }
    return Type {StrTy {}}; 
}

