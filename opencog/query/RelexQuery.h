/**
 * RelexQuery.h
 *
 * Impelement query processing for relex based queries.
 *
 * Copyright (c) 2008 Linas Vepstas <linas@linas.org>
 */

#ifndef _OPENCOG_RELEX_QUERY_H
#define _OPENCOG_RELEX_QUERY_H

#include <map>

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atomspace/FollowLink.h>
#include <opencog/query/PatternMatch.h>

namespace opencog {

class RelexQuery : public PatternMatchCallback
{
	private:
		// Help determine if assertion is a query.
		bool is_qVar(Atom *);
		bool check_for_query(Handle);
		bool assemble_wrapper(Atom *);

		// Convert query into a normal form.
		bool is_ling_rel(Atom *);
		bool is_ling_cncpt(Atom *);
		bool is_cncpt(Atom *);

		bool do_discard;
		bool discard_extra_markup(Atom *);

		// Aid in equivalent node identification.
		bool is_word_instance(Atom *, const char *);

	protected:
		FollowLink fl;

		// Aid in equivalent node identification.
		bool concept_match(Atom *, Atom *);

		// create the predicate
		virtual bool assemble_predicate(Atom *);
		virtual bool find_vars(Handle);

		void add_to_predicate(Handle);
		void add_to_vars(Handle);

		// normalized predicates
		std::vector<Handle> normed_predicate;
		std::vector<Handle> bound_vars;

		// solver
		PatternMatch *pm;

	public:
		RelexQuery(void);
		virtual ~RelexQuery();

		virtual bool is_query(Handle);
		virtual void solve(AtomSpace *, Handle);

		/* Callbacks called from PatternMatch */
		virtual bool node_match(Atom *, Atom *);
		virtual bool solution(std::map<Handle, Handle> &pred_soln,
		                      std::map<Handle, Handle> &var_soln);
};

} // namespace opencog

#endif // _OPENCOG_RELEX_QUERY_H
