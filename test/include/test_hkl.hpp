#pragma once

#include "agizmo/evaluation.hpp"
#include "test_gff.hpp"
#include "test_region.hpp"
#include "test_regionseq.hpp"

using namespace AGizmo::Evaluation;

namespace TestHKL {
int perform_tests(bool verbose);
}

// namespace EvalDOGIToys {
// using General::Stats;
// Stats check_gff3_str_escape(bool verbose);
// Stats check_gff3_str_clean(bool verbose);
// Stats check_DOGI_basic(QString db_name, bool verbose);
// Stats check_DOGI_populate(QString db_name, QString config_file, bool
// verbose); Stats check_DOGI_hpa_expression(const QString &db_name,
//                                const QString expression, bool verbose);
// Stats check_Dogi_mapper_uniprot(const QString &db_name, const QString
// file_name,
//                                bool verbose);
// Stats check_DOGI_go_terms(const QString &db_name, const QString file_name,
//                          bool verbose);
// Stats check_DOGI_go_annotations(const QString &db_name, const QString
// file_name,
//                                bool verbose);

// Stats check_DOGI_gene_ontology(const QString &db_name, const QString
// go_terms,
//                               const QString &annotations, bool verbose);
// Stats check_DOGI_mapper(const QString &db_name, const QString &database_from,
//                        const QString &database_to, const QString &file_name,
//                        bool verbose);
// Stats check_DOGI_genome(const QString &db_name, const QString &id_database,
//                        const QString &masking, const QString &file_name,
//                        bool verbose);
// Stats check_DOGI_GVF(const QString &db_name, const QString &id_database,
//                     const QString &file_name, bool verbose);
// Stats check_DOGI_Regulatory(const QString &db_name, const QString
// &id_database,
//                            const QString &file_name, bool verbose);

// Stats eval_dogitoys(bool verbose = false);
//} // namespace EvalDOGIToys
