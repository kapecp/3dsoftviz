/* -*- mode: C -*-  */
/* 
   IGraph library.
   Copyright (C) 2007  Gabor Csardi <csardi@rmki.kfki.hu>
   MTA RMKI, Konkoly-Thege Miklos st. 29-33, Budapest 1121, Hungary
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc.,  51 Franklin Street, Fifth Floor, Boston, MA 
   02110-1301 USA

*/

#define _GNU_SOURCE

#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#include "igraph.h"

/* ------------------------------------------------------------------------ */
/* Conversion & other common functions first                                */
/* ------------------------------------------------------------------------ */


int shell_igraph_usage(int argc, char **argv) {
  printf("Command line interface to igraph\n");
  return 0;
}

int shell_skip_whitespace(FILE *fin) {
  int c;
  c=fgetc(fin);
  while (isspace(c)) {
    c=fgetc(fin);
  }
  if (c!=EOF) { ungetc(c, fin); }
  return 0;
}

FILE *shell_open_file(const char *where, const char *mode) {  
  static const char *input="input";
  static const char *output="output";
  FILE *f;

  if (!strcmp(where, "-")) {
    if (!strcmp(mode, "r")) {
      return stdin;
    } else if (!strcmp(mode, "w")) {
      return stdout;
    }
  }

  f=fopen(where, mode);
  if (!f) {
    fprintf(stderr, "Cannot open %s file: `%s'\n", 
	    !strcmp(mode, "r") ? input : output, where);
    exit(1);
  }
  return f;
} 

int shell_read_graph(igraph_t *graph, const char *where) {
  FILE *fin=shell_open_file(where, "r");
  igraph_read_graph_graphml(graph, fin, 0);
  fclose(fin);
  return 0;
}

int shell_write_graph(const igraph_t *graph, const char *where) {
  FILE *fout=shell_open_file(where, "w");
  igraph_write_graph_graphml(graph, fout);
  fclose(fout);
  return 0;
}

int shell_read_vector(igraph_vector_t *v, const char *where) {
  FILE *fin=shell_open_file(where, "r");
  igraph_real_t n;
  int ret=1;
  igraph_vector_init(v, 0);
  while (ret > 0) {
    ret=fscanf(fin, "%lf", &n);
    if (ret > 0) { 
      igraph_vector_push_back(v, n);
    }
  }
  fclose(fin);
  if (ret != EOF) {
    fprintf(stderr, "Error reading vector from file: `%s'\n", where);
    exit(1);
  }

  return 0;
}

int shell_write_a_vector(const igraph_vector_t *v, FILE *fout, const char *where) {
  int ret=1;
  long int i, n=igraph_vector_size(v);
  if (n>0) { ret=fprintf(fout, "%g", VECTOR(*v)[0]); }
  if (ret <= 0) { 
    fprintf(stderr, "Cannot write vector to `%s'\n", where);
    fclose(fout);
    exit(1);
  }
  for (i=1; i<n; i++) {
    ret=fprintf(fout, " %g", VECTOR(*v)[i]);
    if (ret <= 0) { 
      fprintf(stderr, "Cannot write vector to `%s'\n", where);
      fclose(fout);
      exit(1);
    }
  }
  fprintf(fout, "\n");
  return 0;
}  

int shell_write_vector(const igraph_vector_t *v, const char *where) {
  FILE *fout=shell_open_file(where, "w");
  shell_write_a_vector(v, fout, where);
  fclose(fout);  
  return 0;
}

int shell_read_a_matrix(igraph_matrix_t *m, FILE *fin, const char *where) {
  long int ncol, nrow, i, j;
  igraph_real_t n;
  int ret;
  if (2 != fscanf(fin, "%li %li", &nrow, &ncol)) {
    fprintf(stderr, "Error reading matrix from file '%s'\n", where);
    fclose(fin);
    exit(1);
  }
  igraph_matrix_init(m,nrow,ncol);
  for (i=0; i<nrow; i++) {
    for (j=0; j<ncol; j++) {
      ret=fscanf(fin, "%lf", &n);
      if (ret <= 0) { 
	fprintf(stderr, "Error reading matrix from file '%s'\n", where);
	fclose(fin);
	exit(1);
      }
      MATRIX(*m, i, j)=n;
    }
  }
  return 0;
}

int shell_read_matrix(igraph_matrix_t *m, const char *where) {
  FILE *fin=shell_open_file(where, "r");
  shell_read_a_matrix(m, fin, where);
  fclose(fin);      
  return 0;
}

int shell_write_matrix(const igraph_matrix_t *m, const char *where) {
  FILE *fout=shell_open_file(where, "w");
  long int nrow=igraph_matrix_nrow(m);
  long int ncol=igraph_matrix_ncol(m);
  long int i, j;
    
  if (0>fprintf(fout, "%li %li\n", nrow, ncol)) {
    fprintf(stderr, "Error writing matrix to file '%s'.\n", where);
    fclose(fout);
    exit(1);
  }

  for (i=0; i<nrow; i++) {
    for (j=0; j<ncol; j++) {
      if (j!=0) { fprintf(fout, " "); }
      if (0>fprintf(fout, "%g", MATRIX(*m, i, j))) {
	fprintf(stderr, "Error writing matrix to file '%s'.\n", where);
	fclose(fout);
	exit(1);
      }
    }
    fprintf(fout, "\n");
  }
  fclose(fout);
  
  return 0;
}

int shell_read_integer(igraph_integer_t *n, const char *where) {
  long int nn;
  int ret=sscanf(where, "%li", &nn);
  if (ret == EOF || ret == 0) {
    fprintf(stderr, "Error, cannot interpret '%s' as integer\n", where);
    exit(1);
  }
  *n=nn;
  return 0;
}

int shell_write_integer(igraph_integer_t n, const char *where) {
  FILE *fout=shell_open_file(where, "w");
  int ret=fprintf(fout, "%li\n", (long int)n);
  if (ret <= 0) { 
    fprintf(stderr, "Cannot write integer to '%s'\n", where);
    fclose(fout);
    exit(1);
  }
  return 0;
}

int shell_read_boolean(igraph_bool_t *b, const char *where) {
  if (strlen(where)==0 || 
      where[0]=='0' ||
      where[0]=='F' ||
      where[0]=='f') {
    *b=0;
  } else {
    *b=1;
  }
  return 0;
}

int shell_write_boolean(igraph_bool_t b, const char *where) {
  int bb= b==0 ? 0 : 1;
  FILE *fout=shell_open_file(where, "w");
  int ret=fprintf(fout, "%i\n", bb);
  if (ret <= 0) { 
    fprintf(stderr, "Cannot write integer to '%s'\n", where);
    fclose(fout);
    exit(1);
  }  
  return 0;
}

int shell_read_real(igraph_real_t *b, const char *where) {
  int ret=sscanf(where, "%lf", b);
  if (ret == EOF || ret == 0) {
    fprintf(stderr, "Error, cannot interpret '%s' as real\n", where);
    exit(1);
  }  
  return 0;
}

int shell_write_real(igraph_real_t b, const char *where) {
  FILE *fout=shell_open_file(where, "w");
  int ret=fprintf(fout, "%g\n", (double)b);
  if (ret <= 0) { 
    fprintf(stderr, "Cannot write real to `%s'\n", where);
    fclose(fout);
    exit(1);
  }
  return 0;
}

int shell_read_enum(void* value, const char *where, ...) {
  int result=-1, *p=value;
  va_list args;
  va_start(args, where);
  
  while (1) {
    char *name=va_arg(args, char*);
    int code;
    if (name) {
      code=va_arg(args, int);
      if (!strcmp(optarg, name)) {
	result=code;
	break;
      }
    } else {
      break;
    }
  }
  
  if (result==-1) {
    fprintf(stderr, "Cannot interpret argument: '%s'.\n", where);
    exit(1);
  }   

  *p=result;

  return 0;
}

int shell_read_int(int *value, const char *where) {
  long int li=strtol(where, 0, 10);
  if (errno || li<INT_MIN || li>INT_MAX) {
    fprintf(stderr, "Integer too small/big: '%s'.\n", where);
  }
  *value=li;
  return 0;
}

int shell_read_longint(long int *value, const char *where) {
  long int li=strtol(where, 0, 10);
  if (errno) {
    fprintf(stderr, "Long integer too small/big: '%s'.\n", where);
  }
  *value=li;
  return 0;
}

int shell_read_file(FILE **file, const char *where, const char *mode) {
  *file=fopen(where, mode);
  if (!*file) {
    fprintf(stderr, "Cannot open file '%s'\n", where);
  }
  return 0;
}

int shell_read_matrixlist(igraph_vector_ptr_t *list, const char *where) {
  FILE *fin=shell_open_file(where, "r");
  igraph_vector_ptr_init(list, 0);
  shell_skip_whitespace(fin);
  while (!feof(fin)) {
    igraph_matrix_t *m=malloc(sizeof(igraph_matrix_t));
    shell_read_a_matrix(m, fin, where);
    igraph_vector_ptr_push_back(list, m);
    shell_skip_whitespace(fin);
  }
  fclose(fin);
  return 0;
}

int shell_read_graphlist(igraph_vector_ptr_t *list, const char *where) {
  FILE *fin=shell_open_file(where, "r");
  igraph_vector_ptr_init(list, 0);
  shell_skip_whitespace(fin);
  while (!feof(fin)) {
    igraph_t *g=malloc(sizeof(igraph_t));
    igraph_read_graph_graphml(g, fin, 0);
    igraph_vector_ptr_push_back(list, g);
    shell_skip_whitespace(fin);
  }
  fclose(fin);
  return 0;
}

int shell_read_strvector(igraph_strvector_t *str, const char *where) {
  FILE *fin=shell_open_file(where, "r");
  char *buffer=calloc(1000, sizeof(char));
  size_t size;
  int c;
  igraph_strvector_init(str, 0);
  while(1) {
    getline(&buffer, &size, fin);
    igraph_strvector_add(str, buffer);
    c=getc(fin);
    if (!feof(fin)) {
      ungetc(c, fin);
    } else {
      break;
    }
  }
  fclose(fin);
  free(buffer);
  
  return 0;
}

int shell_write_vectorlist(igraph_vector_ptr_t *v, const char *where) {
  FILE *fout=shell_open_file(where, "w");
  long int i, n=igraph_vector_ptr_size(v);
  for (i=0; i<n; i++) {
    shell_write_a_vector(VECTOR(*v)[i], fout, where);
  }
  fclose(fout);
  return 0;
}

int shell_write_vector_bool(igraph_vector_bool_t *v, const char *where) {
  FILE *fout=shell_open_file(where, "w");
  int ret=1;
  long int i, n=igraph_vector_bool_size(v);
  if (n>0) { ret=fprintf(fout, "%i", VECTOR(*v)[0]==0 ? 0 : 1); }
  if (ret <= 0) { 
    fprintf(stderr, "Cannot write vector to `%s'\n", where);
    fclose(fout);
    exit(1);
  }
  for (i=1; i<n; i++) {
    ret=fprintf(fout, " %i", VECTOR(*v)[i]==0 ? 0 : 1);
    if (ret <= 0) { 
      fprintf(stderr, "Cannot write vector to `%s'\n", where);
      fclose(fout);
      exit(1);
    }
  }
  fprintf(fout, "\n");
  fclose(fout);
  return 0;
}  

int shell_write_graphlist(igraph_vector_ptr_t *v, const char *where) {
  FILE *fout=shell_open_file(where, "w");
  long int i, n=igraph_vector_ptr_size(v);
  for (i=0; i<n; i++) {
    igraph_write_graph_graphml(VECTOR(*v)[i], fout);
  }
  fclose(fout);
  return 0;
}

int shell_free_graphlist(igraph_vector_ptr_t *v) {
  long int i, n=igraph_vector_ptr_size(v);
  for (i=0; i<n; i++) {
    igraph_destroy(VECTOR(*v)[i]);
    free(VECTOR(*v)[i]);
    VECTOR(*v)[i]=0;
  }
  return 0;
}

int shell_free_matrixlist(igraph_vector_ptr_t *v) {
  long int i, n=igraph_vector_ptr_size(v);
  for (i=0; i<n; i++) {
    igraph_matrix_destroy(VECTOR(*v)[i]);
    free(VECTOR(*v)[i]);
    VECTOR(*v)[i]=0;
  }
  return 0;
}

int shell_free_vectorlist(igraph_vector_ptr_t *v) {
  long int i, n=igraph_vector_ptr_size(v);
  for (i=0; i<n; i++) {
    igraph_vector_destroy(VECTOR(*v)[i]);
    free(VECTOR(*v)[i]);
    VECTOR(*v)[i]=0;
  }
  return 0;
}

/* ------------------------------------------------------------------------ */
/* Stimulus generated interface next                                        */
/* ------------------------------------------------------------------------ */


/* Function prototypes first */

int shell_igraph_empty(int argc, char **argv);
int shell_igraph_add_edges(int argc, char **argv);
int shell_igraph_add_vertices(int argc, char **argv);
int shell_igraph_delete_edges(int argc, char **argv);
int shell_igraph_delete_vertices(int argc, char **argv);
int shell_igraph_vcount(int argc, char **argv);
int shell_igraph_ecount(int argc, char **argv);
int shell_igraph_neighbors(int argc, char **argv);
int shell_igraph_is_directed(int argc, char **argv);
int shell_igraph_degree(int argc, char **argv);
int shell_igraph_edge(int argc, char **argv);
int shell_igraph_edges(int argc, char **argv);
int shell_igraph_get_eid(int argc, char **argv);
int shell_igraph_get_eids(int argc, char **argv);
int shell_igraph_adjacent(int argc, char **argv);
int shell_igraph_create(int argc, char **argv);
int shell_igraph_adjacency(int argc, char **argv);
int shell_igraph_star(int argc, char **argv);
int shell_igraph_lattice(int argc, char **argv);
int shell_igraph_ring(int argc, char **argv);
int shell_igraph_tree(int argc, char **argv);
int shell_igraph_full(int argc, char **argv);
int shell_igraph_full_citation(int argc, char **argv);
int shell_igraph_atlas(int argc, char **argv);
int shell_igraph_extended_chordal_ring(int argc, char **argv);
int shell_igraph_connect_neighborhood(int argc, char **argv);
int shell_igraph_linegraph(int argc, char **argv);
int shell_igraph_de_bruijn(int argc, char **argv);
int shell_igraph_kautz(int argc, char **argv);
int shell_igraph_famous(int argc, char **argv);
int shell_igraph_lcf_vector(int argc, char **argv);
int shell_igraph_adjlist(int argc, char **argv);
int shell_igraph_full_bipartite(int argc, char **argv);
int shell_igraph_barabasi_game(int argc, char **argv);
int shell_igraph_nonlinear_barabasi_game(int argc, char **argv);
int shell_igraph_erdos_renyi_game_gnp(int argc, char **argv);
int shell_igraph_erdos_renyi_game_gnm(int argc, char **argv);
int shell_igraph_degree_sequence_game(int argc, char **argv);
int shell_igraph_growing_random_game(int argc, char **argv);
int shell_igraph_barabasi_aging_game(int argc, char **argv);
int shell_igraph_recent_degree_game(int argc, char **argv);
int shell_igraph_recent_degree_aging_game(int argc, char **argv);
int shell_igraph_callaway_traits_game(int argc, char **argv);
int shell_igraph_establishment_game(int argc, char **argv);
int shell_igraph_grg_game(int argc, char **argv);
int shell_igraph_preference_game(int argc, char **argv);
int shell_igraph_asymmetric_preference_game(int argc, char **argv);
int shell_igraph_rewire_edges(int argc, char **argv);
int shell_igraph_watts_strogatz_game(int argc, char **argv);
int shell_igraph_lastcit_game(int argc, char **argv);
int shell_igraph_cited_type_game(int argc, char **argv);
int shell_igraph_citing_cited_type_game(int argc, char **argv);
int shell_igraph_forest_fire_game(int argc, char **argv);
int shell_igraph_are_connected(int argc, char **argv);
int shell_igraph_diameter(int argc, char **argv);
int shell_igraph_minimum_spanning_tree_unweighted(int argc, char **argv);
int shell_igraph_minimum_spanning_tree_prim(int argc, char **argv);
int shell_igraph_closeness(int argc, char **argv);
int shell_igraph_closeness_estimate(int argc, char **argv);
int shell_igraph_shortest_paths(int argc, char **argv);
int shell_igraph_get_shortest_paths(int argc, char **argv);
int shell_igraph_get_all_shortest_paths(int argc, char **argv);
int shell_igraph_shortest_paths_dijkstra(int argc, char **argv);
int shell_igraph_shortest_paths_bellman_ford(int argc, char **argv);
int shell_igraph_subcomponent(int argc, char **argv);
int shell_igraph_betweenness(int argc, char **argv);
int shell_igraph_betweenness_estimate(int argc, char **argv);
int shell_igraph_edge_betweenness(int argc, char **argv);
int shell_igraph_edge_betweenness_estimate(int argc, char **argv);
int shell_igraph_pagerank_old(int argc, char **argv);
int shell_igraph_pagerank(int argc, char **argv);
int shell_igraph_rewire(int argc, char **argv);
int shell_igraph_subgraph(int argc, char **argv);
int shell_igraph_average_path_length(int argc, char **argv);
int shell_igraph_path_length_hist(int argc, char **argv);
int shell_igraph_simplify(int argc, char **argv);
int shell_igraph_transitivity_undirected(int argc, char **argv);
int shell_igraph_transitivity_local_undirected(int argc, char **argv);
int shell_igraph_transitivity_avglocal_undirected(int argc, char **argv);
int shell_igraph_reciprocity(int argc, char **argv);
int shell_igraph_constraint(int argc, char **argv);
int shell_igraph_maxdegree(int argc, char **argv);
int shell_igraph_density(int argc, char **argv);
int shell_igraph_neighborhood_size(int argc, char **argv);
int shell_igraph_neighborhood(int argc, char **argv);
int shell_igraph_neighborhood_graphs(int argc, char **argv);
int shell_igraph_topological_sorting(int argc, char **argv);
int shell_igraph_is_loop(int argc, char **argv);
int shell_igraph_is_simple(int argc, char **argv);
int shell_igraph_is_multiple(int argc, char **argv);
int shell_igraph_count_multiple(int argc, char **argv);
int shell_igraph_girth(int argc, char **argv);
int shell_igraph_add_edge(int argc, char **argv);
int shell_igraph_eigenvector_centrality(int argc, char **argv);
int shell_igraph_hub_score(int argc, char **argv);
int shell_igraph_authority_score(int argc, char **argv);
int shell_igraph_arpack_rssolve(int argc, char **argv);
int shell_igraph_arpack_rnsolve(int argc, char **argv);
int shell_igraph_arpack_unpack_complex(int argc, char **argv);
int shell_igraph_unfold_tree(int argc, char **argv);
int shell_igraph_laplacian(int argc, char **argv);
int shell_igraph_is_mutual(int argc, char **argv);
int shell_igraph_avg_nearest_neighbor_degree(int argc, char **argv);
int shell_igraph_strength(int argc, char **argv);
int shell_igraph_bipartite_projection_size(int argc, char **argv);
int shell_igraph_bipartite_projection(int argc, char **argv);
int shell_igraph_create_bipartite(int argc, char **argv);
int shell_igraph_incidence(int argc, char **argv);
int shell_igraph_get_incidence(int argc, char **argv);
int shell_igraph_is_bipartite(int argc, char **argv);
int shell_igraph_clusters(int argc, char **argv);
int shell_igraph_is_connected(int argc, char **argv);
int shell_igraph_decompose(int argc, char **argv);
int shell_igraph_articulation_points(int argc, char **argv);
int shell_igraph_biconnected_components(int argc, char **argv);
int shell_igraph_cliques(int argc, char **argv);
int shell_igraph_largest_cliques(int argc, char **argv);
int shell_igraph_maximal_cliques(int argc, char **argv);
int shell_igraph_clique_number(int argc, char **argv);
int shell_igraph_independent_vertex_sets(int argc, char **argv);
int shell_igraph_largest_independent_vertex_sets(int argc, char **argv);
int shell_igraph_maximal_independent_vertex_sets(int argc, char **argv);
int shell_igraph_independence_number(int argc, char **argv);
int shell_igraph_layout_random(int argc, char **argv);
int shell_igraph_layout_circle(int argc, char **argv);
int shell_igraph_layout_fruchterman_reingold(int argc, char **argv);
int shell_igraph_layout_grid_fruchterman_reingold(int argc, char **argv);
int shell_igraph_layout_kamada_kawai(int argc, char **argv);
int shell_igraph_layout_lgl(int argc, char **argv);
int shell_igraph_layout_reingold_tilford(int argc, char **argv);
int shell_igraph_layout_reingold_tilford_circular(int argc, char **argv);
int shell_igraph_layout_random_3d(int argc, char **argv);
int shell_igraph_layout_sphere(int argc, char **argv);
int shell_igraph_layout_fruchterman_reingold_3d(int argc, char **argv);
int shell_igraph_layout_kamada_kawai_3d(int argc, char **argv);
int shell_igraph_layout_graphopt(int argc, char **argv);
int shell_igraph_layout_drl(int argc, char **argv);
int shell_igraph_layout_drl_3d(int argc, char **argv);
int shell_igraph_layout_merge_dla(int argc, char **argv);
int shell_igraph_cocitation(int argc, char **argv);
int shell_igraph_bibcoupling(int argc, char **argv);
int shell_igraph_similarity_jaccard(int argc, char **argv);
int shell_igraph_similarity_dice(int argc, char **argv);
int shell_igraph_similarity_inverse_log_weighted(int argc, char **argv);
int shell_igraph_community_spinglass(int argc, char **argv);
int shell_igraph_community_spinglass_single(int argc, char **argv);
int shell_igraph_community_walktrap(int argc, char **argv);
int shell_igraph_community_edge_betweenness(int argc, char **argv);
int shell_igraph_community_eb_get_merges(int argc, char **argv);
int shell_igraph_community_fastgreedy(int argc, char **argv);
int shell_igraph_community_to_membership(int argc, char **argv);
int shell_igraph_le_community_to_membership(int argc, char **argv);
int shell_igraph_modularity(int argc, char **argv);
int shell_igraph_community_leading_eigenvector(int argc, char **argv);
int shell_igraph_community_leading_eigenvector_naive(int argc, char **argv);
int shell_igraph_community_leading_eigenvector_step(int argc, char **argv);
int shell_igraph_get_adjacency(int argc, char **argv);
int shell_igraph_get_edgelist(int argc, char **argv);
int shell_igraph_to_directed(int argc, char **argv);
int shell_igraph_to_undirected(int argc, char **argv);
int shell_igraph_read_graph_edgelist(int argc, char **argv);
int shell_igraph_read_graph_ncol(int argc, char **argv);
int shell_igraph_read_graph_lgl(int argc, char **argv);
int shell_igraph_read_graph_pajek(int argc, char **argv);
int shell_igraph_read_graph_graphml(int argc, char **argv);
int shell_igraph_read_graph_dimacs(int argc, char **argv);
int shell_igraph_read_graph_graphdb(int argc, char **argv);
int shell_igraph_read_graph_gml(int argc, char **argv);
int shell_igraph_write_graph_edgelist(int argc, char **argv);
int shell_igraph_write_graph_ncol(int argc, char **argv);
int shell_igraph_write_graph_lgl(int argc, char **argv);
int shell_igraph_write_graph_graphml(int argc, char **argv);
int shell_igraph_write_graph_pajek(int argc, char **argv);
int shell_igraph_write_graph_dimacs(int argc, char **argv);
int shell_igraph_write_graph_gml(int argc, char **argv);
int shell_igraph_write_graph_dot(int argc, char **argv);
int shell_igraph_motifs_randesu(int argc, char **argv);
int shell_igraph_motifs_randesu_estimate(int argc, char **argv);
int shell_igraph_motifs_randesu_no(int argc, char **argv);
int shell_igraph_dyad_census(int argc, char **argv);
int shell_igraph_triad_census(int argc, char **argv);
int shell_igraph_disjoint_union(int argc, char **argv);
int shell_igraph_disjoint_union_many(int argc, char **argv);
int shell_igraph_union(int argc, char **argv);
int shell_igraph_union_many(int argc, char **argv);
int shell_igraph_intersection(int argc, char **argv);
int shell_igraph_intersection_many(int argc, char **argv);
int shell_igraph_difference(int argc, char **argv);
int shell_igraph_complementer(int argc, char **argv);
int shell_igraph_compose(int argc, char **argv);
int shell_igraph_maxflow_value(int argc, char **argv);
int shell_igraph_mincut_value(int argc, char **argv);
int shell_igraph_st_mincut_value(int argc, char **argv);
int shell_igraph_mincut(int argc, char **argv);
int shell_igraph_st_vertex_connectivity(int argc, char **argv);
int shell_igraph_vertex_connectivity(int argc, char **argv);
int shell_igraph_st_edge_connectivity(int argc, char **argv);
int shell_igraph_edge_connectivity(int argc, char **argv);
int shell_igraph_edge_disjoint_paths(int argc, char **argv);
int shell_igraph_vertex_disjoint_paths(int argc, char **argv);
int shell_igraph_adhesion(int argc, char **argv);
int shell_igraph_cohesion(int argc, char **argv);
int shell_igraph_coreness(int argc, char **argv);
int shell_igraph_isoclass(int argc, char **argv);
int shell_igraph_isomorphic(int argc, char **argv);
int shell_igraph_isoclass_subgraph(int argc, char **argv);
int shell_igraph_isoclass_create(int argc, char **argv);
int shell_igraph_isomorphic_vf2(int argc, char **argv);
int shell_igraph_count_isomorphisms_vf2(int argc, char **argv);
int shell_igraph_get_isomorphisms_vf2(int argc, char **argv);
int shell_igraph_subisomorphic_vf2(int argc, char **argv);
int shell_igraph_count_subisomorphisms_vf2(int argc, char **argv);
int shell_igraph_get_subisomorphisms_vf2(int argc, char **argv);
int shell_igraph_isomorphic_34(int argc, char **argv);
int shell_igraph_canonical_permutation(int argc, char **argv);
int shell_igraph_permute_vertices(int argc, char **argv);
int shell_igraph_isomorphic_bliss(int argc, char **argv);
int shell_igraph_automorphisms(int argc, char **argv);
int shell_igraph_running_mean(int argc, char **argv);
int shell_igraph_random_sample(int argc, char **argv);
int shell_igraph_convex_hull(int argc, char **argv);
int shell_igraph_revolver_ml_d(int argc, char **argv);
int shell_igraph_revolver_probs_d(int argc, char **argv);
int shell_igraph_revolver_ml_de(int argc, char **argv);
int shell_igraph_revolver_probs_de(int argc, char **argv);
int shell_igraph_revolver_ml_ade(int argc, char **argv);
int shell_igraph_revolver_probs_ade(int argc, char **argv);
int shell_igraph_revolver_ml_f(int argc, char **argv);
int shell_igraph_revolver_ml_df(int argc, char **argv);
int shell_igraph_revolver_ml_l(int argc, char **argv);
int shell_igraph_revolver_ml_ad(int argc, char **argv);
int shell_igraph_revolver_probs_ad(int argc, char **argv);
int shell_igraph_revolver_ml_D_alpha(int argc, char **argv);
int shell_igraph_revolver_ml_D_alpha_a(int argc, char **argv);
int shell_igraph_revolver_ml_DE_alpha_a(int argc, char **argv);
int shell_igraph_revolver_ml_AD_alpha_a_beta(int argc, char **argv);
int shell_igraph_revolver_ml_AD_dpareto(int argc, char **argv);
int shell_igraph_revolver_ml_AD_dpareto_eval(int argc, char **argv);
int shell_igraph_revolver_ml_ADE_alpha_a_beta(int argc, char **argv);
int shell_igraph_revolver_ml_ADE_dpareto(int argc, char **argv);
int shell_igraph_revolver_ml_ADE_dpareto_eval(int argc, char **argv);
int shell_igraph_revolver_ml_ADE_dpareto_evalf(int argc, char **argv);
int shell_igraph_revolver_probs_ADE_dpareto(int argc, char **argv);
int shell_igraph_convergence_degree(int argc, char **argv);

/* The main function */

int main(int argc, char **argv) {

  const char *base=basename(argv[0]);

  if (!strcasecmp(base, "igraph_empty")) {
    return shell_igraph_empty(argc, argv);
  } else if (!strcasecmp(base, "igraph_add_edges")) {
    return shell_igraph_add_edges(argc, argv);
  } else if (!strcasecmp(base, "igraph_add_vertices")) {
    return shell_igraph_add_vertices(argc, argv);
  } else if (!strcasecmp(base, "igraph_delete_edges")) {
    return shell_igraph_delete_edges(argc, argv);
  } else if (!strcasecmp(base, "igraph_delete_vertices")) {
    return shell_igraph_delete_vertices(argc, argv);
  } else if (!strcasecmp(base, "igraph_vcount")) {
    return shell_igraph_vcount(argc, argv);
  } else if (!strcasecmp(base, "igraph_ecount")) {
    return shell_igraph_ecount(argc, argv);
  } else if (!strcasecmp(base, "igraph_neighbors")) {
    return shell_igraph_neighbors(argc, argv);
  } else if (!strcasecmp(base, "igraph_is_directed")) {
    return shell_igraph_is_directed(argc, argv);
  } else if (!strcasecmp(base, "igraph_degree")) {
    return shell_igraph_degree(argc, argv);
  } else if (!strcasecmp(base, "igraph_edge")) {
    return shell_igraph_edge(argc, argv);
  } else if (!strcasecmp(base, "igraph_edges")) {
    return shell_igraph_edges(argc, argv);
  } else if (!strcasecmp(base, "igraph_get_eid")) {
    return shell_igraph_get_eid(argc, argv);
  } else if (!strcasecmp(base, "igraph_get_eids")) {
    return shell_igraph_get_eids(argc, argv);
  } else if (!strcasecmp(base, "igraph_adjacent")) {
    return shell_igraph_adjacent(argc, argv);
  } else if (!strcasecmp(base, "igraph_create")) {
    return shell_igraph_create(argc, argv);
  } else if (!strcasecmp(base, "igraph_adjacency")) {
    return shell_igraph_adjacency(argc, argv);
  } else if (!strcasecmp(base, "igraph_star")) {
    return shell_igraph_star(argc, argv);
  } else if (!strcasecmp(base, "igraph_lattice")) {
    return shell_igraph_lattice(argc, argv);
  } else if (!strcasecmp(base, "igraph_ring")) {
    return shell_igraph_ring(argc, argv);
  } else if (!strcasecmp(base, "igraph_tree")) {
    return shell_igraph_tree(argc, argv);
  } else if (!strcasecmp(base, "igraph_full")) {
    return shell_igraph_full(argc, argv);
  } else if (!strcasecmp(base, "igraph_full_citation")) {
    return shell_igraph_full_citation(argc, argv);
  } else if (!strcasecmp(base, "igraph_atlas")) {
    return shell_igraph_atlas(argc, argv);
  } else if (!strcasecmp(base, "igraph_extended_chordal_ring")) {
    return shell_igraph_extended_chordal_ring(argc, argv);
  } else if (!strcasecmp(base, "igraph_connect_neighborhood")) {
    return shell_igraph_connect_neighborhood(argc, argv);
  } else if (!strcasecmp(base, "igraph_linegraph")) {
    return shell_igraph_linegraph(argc, argv);
  } else if (!strcasecmp(base, "igraph_de_bruijn")) {
    return shell_igraph_de_bruijn(argc, argv);
  } else if (!strcasecmp(base, "igraph_kautz")) {
    return shell_igraph_kautz(argc, argv);
  } else if (!strcasecmp(base, "igraph_famous")) {
    return shell_igraph_famous(argc, argv);
  } else if (!strcasecmp(base, "igraph_lcf_vector")) {
    return shell_igraph_lcf_vector(argc, argv);
  } else if (!strcasecmp(base, "igraph_adjlist")) {
    return shell_igraph_adjlist(argc, argv);
  } else if (!strcasecmp(base, "igraph_full_bipartite")) {
    return shell_igraph_full_bipartite(argc, argv);
  } else if (!strcasecmp(base, "igraph_barabasi_game")) {
    return shell_igraph_barabasi_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_nonlinear_barabasi_game")) {
    return shell_igraph_nonlinear_barabasi_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_erdos_renyi_game_gnp")) {
    return shell_igraph_erdos_renyi_game_gnp(argc, argv);
  } else if (!strcasecmp(base, "igraph_erdos_renyi_game_gnm")) {
    return shell_igraph_erdos_renyi_game_gnm(argc, argv);
  } else if (!strcasecmp(base, "igraph_degree_sequence_game")) {
    return shell_igraph_degree_sequence_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_growing_random_game")) {
    return shell_igraph_growing_random_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_barabasi_aging_game")) {
    return shell_igraph_barabasi_aging_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_recent_degree_game")) {
    return shell_igraph_recent_degree_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_recent_degree_aging_game")) {
    return shell_igraph_recent_degree_aging_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_callaway_traits_game")) {
    return shell_igraph_callaway_traits_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_establishment_game")) {
    return shell_igraph_establishment_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_grg_game")) {
    return shell_igraph_grg_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_preference_game")) {
    return shell_igraph_preference_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_asymmetric_preference_game")) {
    return shell_igraph_asymmetric_preference_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_rewire_edges")) {
    return shell_igraph_rewire_edges(argc, argv);
  } else if (!strcasecmp(base, "igraph_watts_strogatz_game")) {
    return shell_igraph_watts_strogatz_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_lastcit_game")) {
    return shell_igraph_lastcit_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_cited_type_game")) {
    return shell_igraph_cited_type_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_citing_cited_type_game")) {
    return shell_igraph_citing_cited_type_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_forest_fire_game")) {
    return shell_igraph_forest_fire_game(argc, argv);
  } else if (!strcasecmp(base, "igraph_are_connected")) {
    return shell_igraph_are_connected(argc, argv);
  } else if (!strcasecmp(base, "igraph_diameter")) {
    return shell_igraph_diameter(argc, argv);
  } else if (!strcasecmp(base, "igraph_minimum_spanning_tree_unweighted")) {
    return shell_igraph_minimum_spanning_tree_unweighted(argc, argv);
  } else if (!strcasecmp(base, "igraph_minimum_spanning_tree_prim")) {
    return shell_igraph_minimum_spanning_tree_prim(argc, argv);
  } else if (!strcasecmp(base, "igraph_closeness")) {
    return shell_igraph_closeness(argc, argv);
  } else if (!strcasecmp(base, "igraph_closeness_estimate")) {
    return shell_igraph_closeness_estimate(argc, argv);
  } else if (!strcasecmp(base, "igraph_shortest_paths")) {
    return shell_igraph_shortest_paths(argc, argv);
  } else if (!strcasecmp(base, "igraph_get_shortest_paths")) {
    return shell_igraph_get_shortest_paths(argc, argv);
  } else if (!strcasecmp(base, "igraph_get_all_shortest_paths")) {
    return shell_igraph_get_all_shortest_paths(argc, argv);
  } else if (!strcasecmp(base, "igraph_shortest_paths_dijkstra")) {
    return shell_igraph_shortest_paths_dijkstra(argc, argv);
  } else if (!strcasecmp(base, "igraph_shortest_paths_bellman_ford")) {
    return shell_igraph_shortest_paths_bellman_ford(argc, argv);
  } else if (!strcasecmp(base, "igraph_subcomponent")) {
    return shell_igraph_subcomponent(argc, argv);
  } else if (!strcasecmp(base, "igraph_betweenness")) {
    return shell_igraph_betweenness(argc, argv);
  } else if (!strcasecmp(base, "igraph_betweenness_estimate")) {
    return shell_igraph_betweenness_estimate(argc, argv);
  } else if (!strcasecmp(base, "igraph_edge_betweenness")) {
    return shell_igraph_edge_betweenness(argc, argv);
  } else if (!strcasecmp(base, "igraph_edge_betweenness_estimate")) {
    return shell_igraph_edge_betweenness_estimate(argc, argv);
  } else if (!strcasecmp(base, "igraph_pagerank_old")) {
    return shell_igraph_pagerank_old(argc, argv);
  } else if (!strcasecmp(base, "igraph_pagerank")) {
    return shell_igraph_pagerank(argc, argv);
  } else if (!strcasecmp(base, "igraph_rewire")) {
    return shell_igraph_rewire(argc, argv);
  } else if (!strcasecmp(base, "igraph_subgraph")) {
    return shell_igraph_subgraph(argc, argv);
  } else if (!strcasecmp(base, "igraph_average_path_length")) {
    return shell_igraph_average_path_length(argc, argv);
  } else if (!strcasecmp(base, "igraph_path_length_hist")) {
    return shell_igraph_path_length_hist(argc, argv);
  } else if (!strcasecmp(base, "igraph_simplify")) {
    return shell_igraph_simplify(argc, argv);
  } else if (!strcasecmp(base, "igraph_transitivity_undirected")) {
    return shell_igraph_transitivity_undirected(argc, argv);
  } else if (!strcasecmp(base, "igraph_transitivity_local_undirected")) {
    return shell_igraph_transitivity_local_undirected(argc, argv);
  } else if (!strcasecmp(base, "igraph_transitivity_avglocal_undirected")) {
    return shell_igraph_transitivity_avglocal_undirected(argc, argv);
  } else if (!strcasecmp(base, "igraph_reciprocity")) {
    return shell_igraph_reciprocity(argc, argv);
  } else if (!strcasecmp(base, "igraph_constraint")) {
    return shell_igraph_constraint(argc, argv);
  } else if (!strcasecmp(base, "igraph_maxdegree")) {
    return shell_igraph_maxdegree(argc, argv);
  } else if (!strcasecmp(base, "igraph_density")) {
    return shell_igraph_density(argc, argv);
  } else if (!strcasecmp(base, "igraph_neighborhood_size")) {
    return shell_igraph_neighborhood_size(argc, argv);
  } else if (!strcasecmp(base, "igraph_neighborhood")) {
    return shell_igraph_neighborhood(argc, argv);
  } else if (!strcasecmp(base, "igraph_neighborhood_graphs")) {
    return shell_igraph_neighborhood_graphs(argc, argv);
  } else if (!strcasecmp(base, "igraph_topological_sorting")) {
    return shell_igraph_topological_sorting(argc, argv);
  } else if (!strcasecmp(base, "igraph_is_loop")) {
    return shell_igraph_is_loop(argc, argv);
  } else if (!strcasecmp(base, "igraph_is_simple")) {
    return shell_igraph_is_simple(argc, argv);
  } else if (!strcasecmp(base, "igraph_is_multiple")) {
    return shell_igraph_is_multiple(argc, argv);
  } else if (!strcasecmp(base, "igraph_count_multiple")) {
    return shell_igraph_count_multiple(argc, argv);
  } else if (!strcasecmp(base, "igraph_girth")) {
    return shell_igraph_girth(argc, argv);
  } else if (!strcasecmp(base, "igraph_add_edge")) {
    return shell_igraph_add_edge(argc, argv);
  } else if (!strcasecmp(base, "igraph_eigenvector_centrality")) {
    return shell_igraph_eigenvector_centrality(argc, argv);
  } else if (!strcasecmp(base, "igraph_hub_score")) {
    return shell_igraph_hub_score(argc, argv);
  } else if (!strcasecmp(base, "igraph_authority_score")) {
    return shell_igraph_authority_score(argc, argv);
  } else if (!strcasecmp(base, "igraph_arpack_rssolve")) {
    return shell_igraph_arpack_rssolve(argc, argv);
  } else if (!strcasecmp(base, "igraph_arpack_rnsolve")) {
    return shell_igraph_arpack_rnsolve(argc, argv);
  } else if (!strcasecmp(base, "igraph_arpack_unpack_complex")) {
    return shell_igraph_arpack_unpack_complex(argc, argv);
  } else if (!strcasecmp(base, "igraph_unfold_tree")) {
    return shell_igraph_unfold_tree(argc, argv);
  } else if (!strcasecmp(base, "igraph_laplacian")) {
    return shell_igraph_laplacian(argc, argv);
  } else if (!strcasecmp(base, "igraph_is_mutual")) {
    return shell_igraph_is_mutual(argc, argv);
  } else if (!strcasecmp(base, "igraph_avg_nearest_neighbor_degree")) {
    return shell_igraph_avg_nearest_neighbor_degree(argc, argv);
  } else if (!strcasecmp(base, "igraph_strength")) {
    return shell_igraph_strength(argc, argv);
  } else if (!strcasecmp(base, "igraph_bipartite_projection_size")) {
    return shell_igraph_bipartite_projection_size(argc, argv);
  } else if (!strcasecmp(base, "igraph_bipartite_projection")) {
    return shell_igraph_bipartite_projection(argc, argv);
  } else if (!strcasecmp(base, "igraph_create_bipartite")) {
    return shell_igraph_create_bipartite(argc, argv);
  } else if (!strcasecmp(base, "igraph_incidence")) {
    return shell_igraph_incidence(argc, argv);
  } else if (!strcasecmp(base, "igraph_get_incidence")) {
    return shell_igraph_get_incidence(argc, argv);
  } else if (!strcasecmp(base, "igraph_is_bipartite")) {
    return shell_igraph_is_bipartite(argc, argv);
  } else if (!strcasecmp(base, "igraph_clusters")) {
    return shell_igraph_clusters(argc, argv);
  } else if (!strcasecmp(base, "igraph_is_connected")) {
    return shell_igraph_is_connected(argc, argv);
  } else if (!strcasecmp(base, "igraph_decompose")) {
    return shell_igraph_decompose(argc, argv);
  } else if (!strcasecmp(base, "igraph_articulation_points")) {
    return shell_igraph_articulation_points(argc, argv);
  } else if (!strcasecmp(base, "igraph_biconnected_components")) {
    return shell_igraph_biconnected_components(argc, argv);
  } else if (!strcasecmp(base, "igraph_cliques")) {
    return shell_igraph_cliques(argc, argv);
  } else if (!strcasecmp(base, "igraph_largest_cliques")) {
    return shell_igraph_largest_cliques(argc, argv);
  } else if (!strcasecmp(base, "igraph_maximal_cliques")) {
    return shell_igraph_maximal_cliques(argc, argv);
  } else if (!strcasecmp(base, "igraph_clique_number")) {
    return shell_igraph_clique_number(argc, argv);
  } else if (!strcasecmp(base, "igraph_independent_vertex_sets")) {
    return shell_igraph_independent_vertex_sets(argc, argv);
  } else if (!strcasecmp(base, "igraph_largest_independent_vertex_sets")) {
    return shell_igraph_largest_independent_vertex_sets(argc, argv);
  } else if (!strcasecmp(base, "igraph_maximal_independent_vertex_sets")) {
    return shell_igraph_maximal_independent_vertex_sets(argc, argv);
  } else if (!strcasecmp(base, "igraph_independence_number")) {
    return shell_igraph_independence_number(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_random")) {
    return shell_igraph_layout_random(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_circle")) {
    return shell_igraph_layout_circle(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_fruchterman_reingold")) {
    return shell_igraph_layout_fruchterman_reingold(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_grid_fruchterman_reingold")) {
    return shell_igraph_layout_grid_fruchterman_reingold(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_kamada_kawai")) {
    return shell_igraph_layout_kamada_kawai(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_lgl")) {
    return shell_igraph_layout_lgl(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_reingold_tilford")) {
    return shell_igraph_layout_reingold_tilford(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_reingold_tilford_circular")) {
    return shell_igraph_layout_reingold_tilford_circular(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_random_3d")) {
    return shell_igraph_layout_random_3d(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_sphere")) {
    return shell_igraph_layout_sphere(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_fruchterman_reingold_3d")) {
    return shell_igraph_layout_fruchterman_reingold_3d(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_kamada_kawai_3d")) {
    return shell_igraph_layout_kamada_kawai_3d(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_graphopt")) {
    return shell_igraph_layout_graphopt(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_drl")) {
    return shell_igraph_layout_drl(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_drl_3d")) {
    return shell_igraph_layout_drl_3d(argc, argv);
  } else if (!strcasecmp(base, "igraph_layout_merge_dla")) {
    return shell_igraph_layout_merge_dla(argc, argv);
  } else if (!strcasecmp(base, "igraph_cocitation")) {
    return shell_igraph_cocitation(argc, argv);
  } else if (!strcasecmp(base, "igraph_bibcoupling")) {
    return shell_igraph_bibcoupling(argc, argv);
  } else if (!strcasecmp(base, "igraph_similarity_jaccard")) {
    return shell_igraph_similarity_jaccard(argc, argv);
  } else if (!strcasecmp(base, "igraph_similarity_dice")) {
    return shell_igraph_similarity_dice(argc, argv);
  } else if (!strcasecmp(base, "igraph_similarity_inverse_log_weighted")) {
    return shell_igraph_similarity_inverse_log_weighted(argc, argv);
  } else if (!strcasecmp(base, "igraph_community_spinglass")) {
    return shell_igraph_community_spinglass(argc, argv);
  } else if (!strcasecmp(base, "igraph_community_spinglass_single")) {
    return shell_igraph_community_spinglass_single(argc, argv);
  } else if (!strcasecmp(base, "igraph_community_walktrap")) {
    return shell_igraph_community_walktrap(argc, argv);
  } else if (!strcasecmp(base, "igraph_community_edge_betweenness")) {
    return shell_igraph_community_edge_betweenness(argc, argv);
  } else if (!strcasecmp(base, "igraph_community_eb_get_merges")) {
    return shell_igraph_community_eb_get_merges(argc, argv);
  } else if (!strcasecmp(base, "igraph_community_fastgreedy")) {
    return shell_igraph_community_fastgreedy(argc, argv);
  } else if (!strcasecmp(base, "igraph_community_to_membership")) {
    return shell_igraph_community_to_membership(argc, argv);
  } else if (!strcasecmp(base, "igraph_le_community_to_membership")) {
    return shell_igraph_le_community_to_membership(argc, argv);
  } else if (!strcasecmp(base, "igraph_modularity")) {
    return shell_igraph_modularity(argc, argv);
  } else if (!strcasecmp(base, "igraph_community_leading_eigenvector")) {
    return shell_igraph_community_leading_eigenvector(argc, argv);
  } else if (!strcasecmp(base, "igraph_community_leading_eigenvector_naive")) {
    return shell_igraph_community_leading_eigenvector_naive(argc, argv);
  } else if (!strcasecmp(base, "igraph_community_leading_eigenvector_step")) {
    return shell_igraph_community_leading_eigenvector_step(argc, argv);
  } else if (!strcasecmp(base, "igraph_get_adjacency")) {
    return shell_igraph_get_adjacency(argc, argv);
  } else if (!strcasecmp(base, "igraph_get_edgelist")) {
    return shell_igraph_get_edgelist(argc, argv);
  } else if (!strcasecmp(base, "igraph_to_directed")) {
    return shell_igraph_to_directed(argc, argv);
  } else if (!strcasecmp(base, "igraph_to_undirected")) {
    return shell_igraph_to_undirected(argc, argv);
  } else if (!strcasecmp(base, "igraph_read_graph_edgelist")) {
    return shell_igraph_read_graph_edgelist(argc, argv);
  } else if (!strcasecmp(base, "igraph_read_graph_ncol")) {
    return shell_igraph_read_graph_ncol(argc, argv);
  } else if (!strcasecmp(base, "igraph_read_graph_lgl")) {
    return shell_igraph_read_graph_lgl(argc, argv);
  } else if (!strcasecmp(base, "igraph_read_graph_pajek")) {
    return shell_igraph_read_graph_pajek(argc, argv);
  } else if (!strcasecmp(base, "igraph_read_graph_graphml")) {
    return shell_igraph_read_graph_graphml(argc, argv);
  } else if (!strcasecmp(base, "igraph_read_graph_dimacs")) {
    return shell_igraph_read_graph_dimacs(argc, argv);
  } else if (!strcasecmp(base, "igraph_read_graph_graphdb")) {
    return shell_igraph_read_graph_graphdb(argc, argv);
  } else if (!strcasecmp(base, "igraph_read_graph_gml")) {
    return shell_igraph_read_graph_gml(argc, argv);
  } else if (!strcasecmp(base, "igraph_write_graph_edgelist")) {
    return shell_igraph_write_graph_edgelist(argc, argv);
  } else if (!strcasecmp(base, "igraph_write_graph_ncol")) {
    return shell_igraph_write_graph_ncol(argc, argv);
  } else if (!strcasecmp(base, "igraph_write_graph_lgl")) {
    return shell_igraph_write_graph_lgl(argc, argv);
  } else if (!strcasecmp(base, "igraph_write_graph_graphml")) {
    return shell_igraph_write_graph_graphml(argc, argv);
  } else if (!strcasecmp(base, "igraph_write_graph_pajek")) {
    return shell_igraph_write_graph_pajek(argc, argv);
  } else if (!strcasecmp(base, "igraph_write_graph_dimacs")) {
    return shell_igraph_write_graph_dimacs(argc, argv);
  } else if (!strcasecmp(base, "igraph_write_graph_gml")) {
    return shell_igraph_write_graph_gml(argc, argv);
  } else if (!strcasecmp(base, "igraph_write_graph_dot")) {
    return shell_igraph_write_graph_dot(argc, argv);
  } else if (!strcasecmp(base, "igraph_motifs_randesu")) {
    return shell_igraph_motifs_randesu(argc, argv);
  } else if (!strcasecmp(base, "igraph_motifs_randesu_estimate")) {
    return shell_igraph_motifs_randesu_estimate(argc, argv);
  } else if (!strcasecmp(base, "igraph_motifs_randesu_no")) {
    return shell_igraph_motifs_randesu_no(argc, argv);
  } else if (!strcasecmp(base, "igraph_dyad_census")) {
    return shell_igraph_dyad_census(argc, argv);
  } else if (!strcasecmp(base, "igraph_triad_census")) {
    return shell_igraph_triad_census(argc, argv);
  } else if (!strcasecmp(base, "igraph_disjoint_union")) {
    return shell_igraph_disjoint_union(argc, argv);
  } else if (!strcasecmp(base, "igraph_disjoint_union_many")) {
    return shell_igraph_disjoint_union_many(argc, argv);
  } else if (!strcasecmp(base, "igraph_union")) {
    return shell_igraph_union(argc, argv);
  } else if (!strcasecmp(base, "igraph_union_many")) {
    return shell_igraph_union_many(argc, argv);
  } else if (!strcasecmp(base, "igraph_intersection")) {
    return shell_igraph_intersection(argc, argv);
  } else if (!strcasecmp(base, "igraph_intersection_many")) {
    return shell_igraph_intersection_many(argc, argv);
  } else if (!strcasecmp(base, "igraph_difference")) {
    return shell_igraph_difference(argc, argv);
  } else if (!strcasecmp(base, "igraph_complementer")) {
    return shell_igraph_complementer(argc, argv);
  } else if (!strcasecmp(base, "igraph_compose")) {
    return shell_igraph_compose(argc, argv);
  } else if (!strcasecmp(base, "igraph_maxflow_value")) {
    return shell_igraph_maxflow_value(argc, argv);
  } else if (!strcasecmp(base, "igraph_mincut_value")) {
    return shell_igraph_mincut_value(argc, argv);
  } else if (!strcasecmp(base, "igraph_st_mincut_value")) {
    return shell_igraph_st_mincut_value(argc, argv);
  } else if (!strcasecmp(base, "igraph_mincut")) {
    return shell_igraph_mincut(argc, argv);
  } else if (!strcasecmp(base, "igraph_st_vertex_connectivity")) {
    return shell_igraph_st_vertex_connectivity(argc, argv);
  } else if (!strcasecmp(base, "igraph_vertex_connectivity")) {
    return shell_igraph_vertex_connectivity(argc, argv);
  } else if (!strcasecmp(base, "igraph_st_edge_connectivity")) {
    return shell_igraph_st_edge_connectivity(argc, argv);
  } else if (!strcasecmp(base, "igraph_edge_connectivity")) {
    return shell_igraph_edge_connectivity(argc, argv);
  } else if (!strcasecmp(base, "igraph_edge_disjoint_paths")) {
    return shell_igraph_edge_disjoint_paths(argc, argv);
  } else if (!strcasecmp(base, "igraph_vertex_disjoint_paths")) {
    return shell_igraph_vertex_disjoint_paths(argc, argv);
  } else if (!strcasecmp(base, "igraph_adhesion")) {
    return shell_igraph_adhesion(argc, argv);
  } else if (!strcasecmp(base, "igraph_cohesion")) {
    return shell_igraph_cohesion(argc, argv);
  } else if (!strcasecmp(base, "igraph_coreness")) {
    return shell_igraph_coreness(argc, argv);
  } else if (!strcasecmp(base, "igraph_isoclass")) {
    return shell_igraph_isoclass(argc, argv);
  } else if (!strcasecmp(base, "igraph_isomorphic")) {
    return shell_igraph_isomorphic(argc, argv);
  } else if (!strcasecmp(base, "igraph_isoclass_subgraph")) {
    return shell_igraph_isoclass_subgraph(argc, argv);
  } else if (!strcasecmp(base, "igraph_isoclass_create")) {
    return shell_igraph_isoclass_create(argc, argv);
  } else if (!strcasecmp(base, "igraph_isomorphic_vf2")) {
    return shell_igraph_isomorphic_vf2(argc, argv);
  } else if (!strcasecmp(base, "igraph_count_isomorphisms_vf2")) {
    return shell_igraph_count_isomorphisms_vf2(argc, argv);
  } else if (!strcasecmp(base, "igraph_get_isomorphisms_vf2")) {
    return shell_igraph_get_isomorphisms_vf2(argc, argv);
  } else if (!strcasecmp(base, "igraph_subisomorphic_vf2")) {
    return shell_igraph_subisomorphic_vf2(argc, argv);
  } else if (!strcasecmp(base, "igraph_count_subisomorphisms_vf2")) {
    return shell_igraph_count_subisomorphisms_vf2(argc, argv);
  } else if (!strcasecmp(base, "igraph_get_subisomorphisms_vf2")) {
    return shell_igraph_get_subisomorphisms_vf2(argc, argv);
  } else if (!strcasecmp(base, "igraph_isomorphic_34")) {
    return shell_igraph_isomorphic_34(argc, argv);
  } else if (!strcasecmp(base, "igraph_canonical_permutation")) {
    return shell_igraph_canonical_permutation(argc, argv);
  } else if (!strcasecmp(base, "igraph_permute_vertices")) {
    return shell_igraph_permute_vertices(argc, argv);
  } else if (!strcasecmp(base, "igraph_isomorphic_bliss")) {
    return shell_igraph_isomorphic_bliss(argc, argv);
  } else if (!strcasecmp(base, "igraph_automorphisms")) {
    return shell_igraph_automorphisms(argc, argv);
  } else if (!strcasecmp(base, "igraph_running_mean")) {
    return shell_igraph_running_mean(argc, argv);
  } else if (!strcasecmp(base, "igraph_random_sample")) {
    return shell_igraph_random_sample(argc, argv);
  } else if (!strcasecmp(base, "igraph_convex_hull")) {
    return shell_igraph_convex_hull(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_d")) {
    return shell_igraph_revolver_ml_d(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_probs_d")) {
    return shell_igraph_revolver_probs_d(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_de")) {
    return shell_igraph_revolver_ml_de(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_probs_de")) {
    return shell_igraph_revolver_probs_de(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_ade")) {
    return shell_igraph_revolver_ml_ade(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_probs_ade")) {
    return shell_igraph_revolver_probs_ade(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_f")) {
    return shell_igraph_revolver_ml_f(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_df")) {
    return shell_igraph_revolver_ml_df(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_l")) {
    return shell_igraph_revolver_ml_l(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_ad")) {
    return shell_igraph_revolver_ml_ad(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_probs_ad")) {
    return shell_igraph_revolver_probs_ad(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_D_alpha")) {
    return shell_igraph_revolver_ml_D_alpha(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_D_alpha_a")) {
    return shell_igraph_revolver_ml_D_alpha_a(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_DE_alpha_a")) {
    return shell_igraph_revolver_ml_DE_alpha_a(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_AD_alpha_a_beta")) {
    return shell_igraph_revolver_ml_AD_alpha_a_beta(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_AD_dpareto")) {
    return shell_igraph_revolver_ml_AD_dpareto(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_AD_dpareto_eval")) {
    return shell_igraph_revolver_ml_AD_dpareto_eval(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_ADE_alpha_a_beta")) {
    return shell_igraph_revolver_ml_ADE_alpha_a_beta(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_ADE_dpareto")) {
    return shell_igraph_revolver_ml_ADE_dpareto(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_ADE_dpareto_eval")) {
    return shell_igraph_revolver_ml_ADE_dpareto_eval(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_ml_ADE_dpareto_evalf")) {
    return shell_igraph_revolver_ml_ADE_dpareto_evalf(argc, argv);
  } else if (!strcasecmp(base, "igraph_revolver_probs_ADE_dpareto")) {
    return shell_igraph_revolver_probs_ADE_dpareto(argc, argv);
  } else if (!strcasecmp(base, "igraph_convergence_degree")) {
    return shell_igraph_convergence_degree(argc, argv);
  } else {
    printf("Unknown function, exiting\n");
  }

  shell_igraph_usage(argc, argv);
  return 0;

}

/* The functions themselves at last */

/*-------------------------------------------/
/ igraph_empty                               /
/-------------------------------------------*/
void shell_igraph_empty_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_empty(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n=0;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "directed",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[1]=2;
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 3:
      shell_igraph_empty_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_empty(&graph, n, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_add_edges                           /
/-------------------------------------------*/
void shell_igraph_add_edges_usage(char **argv) {
  printf("%s --graph=<graph> --graph-out=<graph-out> --edges=<edges>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_add_edges(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t edges;

  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "graph-out",required_argument,0,1 },
                                   { "edges",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* graph-out */
      shell_arg_graph=strdup(optarg);
      break;
    case 2: /* edges */
      shell_read_vector(&edges, optarg);
      break;
    case 3:
      shell_igraph_add_edges_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_add_edges(&graph, &edges, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&edges);

  return 0;
}

/*-------------------------------------------/
/ igraph_add_vertices                        /
/-------------------------------------------*/
void shell_igraph_add_vertices_usage(char **argv) {
  printf("%s --graph=<graph> --graph-out=<graph-out> --nv=<nv>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_add_vertices(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nv;

  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "graph-out",required_argument,0,1 },
                                   { "nv",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* graph-out */
      shell_arg_graph=strdup(optarg);
      break;
    case 2: /* nv */
      shell_read_integer(&nv, optarg);
      break;
    case 3:
      shell_igraph_add_vertices_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_add_vertices(&graph, nv, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_delete_edges                        /
/-------------------------------------------*/
void shell_igraph_delete_edges_usage(char **argv) {
  printf("%s --graph=<graph> --graph-out=<graph-out> --edges=<edges>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_delete_edges(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t v_edges; igraph_es_t edges;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "graph-out",required_argument,0,1 },
                                   { "edges",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* graph-out */
      shell_arg_graph=strdup(optarg);
      break;
    case 2: /* edges */
      shell_read_vector(&v_edges, optarg); igraph_es_vector(&edges, &v_edges);
      break;
    case 3:
      shell_igraph_delete_edges_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_delete_edges(&graph, edges);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  if (!igraph_es_is_all(&edges)) { igraph_es_destroy(&edges); }

  return 0;
}

/*-------------------------------------------/
/ igraph_delete_vertices                     /
/-------------------------------------------*/
void shell_igraph_delete_vertices_usage(char **argv) {
  printf("%s --graph=<graph> --graph-out=<graph-out> --vertices=<vertices>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_delete_vertices(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t v_vertices; igraph_vs_t vertices;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "graph-out",required_argument,0,1 },
                                   { "vertices",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* graph-out */
      shell_arg_graph=strdup(optarg);
      break;
    case 2: /* vertices */
      shell_read_vector(&v_vertices, optarg); igraph_vs_vector(&vertices, &v_vertices);
      break;
    case 3:
      shell_igraph_delete_vertices_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_delete_vertices(&graph, vertices);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  if (!igraph_vs_is_all(&vertices)) { igraph_vector_destroy(&v_vertices); }

  return 0;
}

/*-------------------------------------------/
/ igraph_vcount                              /
/-------------------------------------------*/
void shell_igraph_vcount_usage(char **argv) {
  printf("%s --graph=<graph>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_vcount(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t shell_result;
  char *shell_arg_shell_result="-";

  int shell_seen[1];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "help",no_argument,0,1 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 1*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1:
      shell_igraph_vcount_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<1; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_vcount(&graph);

  /* Write the result */
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_ecount                              /
/-------------------------------------------*/
void shell_igraph_ecount_usage(char **argv) {
  printf("%s --graph=<graph>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_ecount(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t shell_result;
  char *shell_arg_shell_result="-";

  int shell_seen[1];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "help",no_argument,0,1 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 1*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1:
      shell_igraph_ecount_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<1; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_ecount(&graph);

  /* Write the result */
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_neighbors                           /
/-------------------------------------------*/
void shell_igraph_neighbors_usage(char **argv) {
  printf("%s --graph=<graph> --neis=<neis> --vid=<vid> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_neighbors(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t neis;
  igraph_integer_t vid;
  igraph_neimode_t mode=IGRAPH_ALL;
  char* shell_arg_neis=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "neis",required_argument,0,1 },
                                   { "vid",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* neis */
      shell_arg_neis=strdup(optarg); 
  igraph_vector_init(&neis, 0);
      break;
    case 2: /* vid */
      shell_read_integer(&vid, optarg);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4:
      shell_igraph_neighbors_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_neighbors(&graph, &neis, vid, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&neis, shell_arg_neis); 
  igraph_vector_destroy(&neis);

  return 0;
}

/*-------------------------------------------/
/ igraph_is_directed                         /
/-------------------------------------------*/
void shell_igraph_is_directed_usage(char **argv) {
  printf("%s --graph=<graph>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_is_directed(int argc, char **argv) {

  igraph_t graph;
  igraph_bool_t shell_result;
  char *shell_arg_shell_result="-";

  int shell_seen[1];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "help",no_argument,0,1 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 1*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1:
      shell_igraph_is_directed_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<1; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_is_directed(&graph);

  /* Write the result */
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_degree                              /
/-------------------------------------------*/
void shell_igraph_degree_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --mode=<mode> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_degree(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_neimode_t mode=IGRAPH_ALL;
  igraph_bool_t loops;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "loops",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 5:
      shell_igraph_degree_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_degree(&graph, &res, vids, mode, loops);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_edge                                /
/-------------------------------------------*/
void shell_igraph_edge_usage(char **argv) {
  printf("%s --graph=<graph> --eid=<eid> --from=<from> --to=<to>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_edge(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t eid;
  igraph_integer_t from;
  igraph_integer_t to;
  char* shell_arg_from=0;
  char* shell_arg_to=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "eid",required_argument,0,1 },
                                   { "from",required_argument,0,2 },
                                   { "to",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* eid */
      shell_read_integer(&eid, optarg);
      break;
    case 2: /* from */
      shell_arg_from=strdup(optarg);
      break;
    case 3: /* to */
      shell_arg_to=strdup(optarg);
      break;
    case 4:
      shell_igraph_edge_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_edge(&graph, eid, &from, &to);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(from, shell_arg_from);
  shell_write_integer(to, shell_arg_to);

  return 0;
}

/*-------------------------------------------/
/ igraph_edges                               /
/-------------------------------------------*/
void shell_igraph_edges_usage(char **argv) {
  printf("%s --graph=<graph> --eids=<eids> --edges=<edges>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_edges(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t v_eids; igraph_es_t eids;
  igraph_vector_t edges;
  char* shell_arg_edges=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "eids",required_argument,0,1 },
                                   { "edges",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* eids */
      shell_read_vector(&v_eids, optarg); igraph_es_vector(&eids, &v_eids);
      break;
    case 2: /* edges */
      shell_arg_edges=strdup(optarg); 
  igraph_vector_init(&edges, 0);
      break;
    case 3:
      shell_igraph_edges_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_edges(&graph, eids, &edges);

  /* Write the result */
  igraph_destroy(&graph);
  if (!igraph_es_is_all(&eids)) { igraph_es_destroy(&eids); }
  shell_write_vector(&edges, shell_arg_edges); 
  igraph_vector_destroy(&edges);

  return 0;
}

/*-------------------------------------------/
/ igraph_get_eid                             /
/-------------------------------------------*/
void shell_igraph_get_eid_usage(char **argv) {
  printf("%s --graph=<graph> --eid=<eid> --from=<from> --to=<to> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_get_eid(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t eid;
  igraph_integer_t from;
  igraph_integer_t to;
  igraph_bool_t directed=1;
  char* shell_arg_eid=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "eid",required_argument,0,1 },
                                   { "from",required_argument,0,2 },
                                   { "to",required_argument,0,3 },
                                   { "directed",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* eid */
      shell_arg_eid=strdup(optarg);
      break;
    case 2: /* from */
      shell_read_integer(&from, optarg);
      break;
    case 3: /* to */
      shell_read_integer(&to, optarg);
      break;
    case 4: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 5:
      shell_igraph_get_eid_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_get_eid(&graph, &eid, from, to, directed);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(eid, shell_arg_eid);

  return 0;
}

/*-------------------------------------------/
/ igraph_get_eids                            /
/-------------------------------------------*/
void shell_igraph_get_eids_usage(char **argv) {
  printf("%s --graph=<graph> --eids=<eids> --pairs=<pairs> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_get_eids(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t eids;
  igraph_vector_t pairs;
  igraph_bool_t directed=1;
  char* shell_arg_eids=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "eids",required_argument,0,1 },
                                   { "pairs",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* eids */
      shell_arg_eids=strdup(optarg); 
  igraph_vector_init(&eids, 0);
      break;
    case 2: /* pairs */
      shell_read_vector(&pairs, optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4:
      shell_igraph_get_eids_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_get_eids(&graph, &eids, &pairs, directed);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&eids, shell_arg_eids); 
  igraph_vector_destroy(&eids);
  igraph_vector_destroy(&pairs);

  return 0;
}

/*-------------------------------------------/
/ igraph_adjacent                            /
/-------------------------------------------*/
void shell_igraph_adjacent_usage(char **argv) {
  printf("%s --graph=<graph> --eids=<eids> --vid=<vid> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_adjacent(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t eids;
  igraph_integer_t vid;
  igraph_neimode_t mode=IGRAPH_ALL;
  char* shell_arg_eids=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "eids",required_argument,0,1 },
                                   { "vid",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* eids */
      shell_arg_eids=strdup(optarg); 
  igraph_vector_init(&eids, 0);
      break;
    case 2: /* vid */
      shell_read_integer(&vid, optarg);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4:
      shell_igraph_adjacent_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_adjacent(&graph, &eids, vid, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&eids, shell_arg_eids); 
  igraph_vector_destroy(&eids);

  return 0;
}

/*-------------------------------------------/
/ igraph_create                              /
/-------------------------------------------*/
void shell_igraph_create_usage(char **argv) {
  printf("%s --graph=<graph> --edges=<edges> --n=<n> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_create(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t edges;
  igraph_integer_t n=0;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "edges",required_argument,0,1 },
                                   { "n",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* edges */
      shell_read_vector(&edges, optarg);
      break;
    case 2: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4:
      shell_igraph_create_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_create(&graph, &edges, n, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&edges);

  return 0;
}

/*-------------------------------------------/
/ igraph_adjacency                           /
/-------------------------------------------*/
void shell_igraph_adjacency_usage(char **argv) {
  printf("%s --graph=<graph> --adjmatrix=<adjmatrix> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_adjacency(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t adjmatrix;
  igraph_adjacency_t mode=IGRAPH_ADJ_DIRECTED;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "adjmatrix",required_argument,0,1 },
                                   { "mode",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* adjmatrix */
      shell_read_matrix(&adjmatrix, optarg);
      break;
    case 2: /* mode */
      shell_read_enum(&mode, optarg, "directed", 0, "undirected", 1, 
  "max", 1, "upper", 2, "lower", 3, "min", 4, "plus", 5, 0);
      break;
    case 3:
      shell_igraph_adjacency_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_adjacency(&graph, &adjmatrix, mode);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_matrix_destroy(&adjmatrix);

  return 0;
}

/*-------------------------------------------/
/ igraph_star                                /
/-------------------------------------------*/
void shell_igraph_star_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --mode=<mode> --center=<center>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_star(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_star_mode_t mode=IGRAPH_STAR_OUT;
  igraph_integer_t center=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "mode",required_argument,0,2 },
                                   { "center",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* mode */
      shell_read_enum(&mode, optarg, "out", 0, "in", 1, "undirected", 2, 0);
      break;
    case 3: /* center */
      shell_read_integer(&center, optarg);
      break;
    case 4:
      shell_igraph_star_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_star(&graph, n, mode, center);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_lattice                             /
/-------------------------------------------*/
void shell_igraph_lattice_usage(char **argv) {
  printf("%s --graph=<graph> --dimvector=<dimvector> --nei=<nei> --directed=<directed> --mutual=<mutual> --circular=<circular>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_lattice(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t dimvector;
  igraph_integer_t nei=1;
  igraph_bool_t directed=0;
  igraph_bool_t mutual=0;
  igraph_bool_t circular=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "dimvector",required_argument,0,1 },
                                   { "nei",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "mutual",required_argument,0,4 },
                                   { "circular",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* dimvector */
      shell_read_vector(&dimvector, optarg);
      break;
    case 2: /* nei */
      shell_read_integer(&nei, optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4: /* mutual */
      shell_read_boolean(&mutual, optarg);
      break;
    case 5: /* circular */
      shell_read_boolean(&circular, optarg);
      break;
    case 6:
      shell_igraph_lattice_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_lattice(&graph, &dimvector, nei, directed, mutual, circular);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&dimvector);

  return 0;
}

/*-------------------------------------------/
/ igraph_ring                                /
/-------------------------------------------*/
void shell_igraph_ring_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --directed=<directed> --mutual=<mutual> --circular=<circular>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_ring(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_bool_t directed=0;
  igraph_bool_t mutual=0;
  igraph_bool_t circular=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "directed",required_argument,0,2 },
                                   { "mutual",required_argument,0,3 },
                                   { "circular",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 3: /* mutual */
      shell_read_boolean(&mutual, optarg);
      break;
    case 4: /* circular */
      shell_read_boolean(&circular, optarg);
      break;
    case 5:
      shell_igraph_ring_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_ring(&graph, n, directed, mutual, circular);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_tree                                /
/-------------------------------------------*/
void shell_igraph_tree_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --children=<children> --type=<type>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_tree(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_integer_t children=2;
  igraph_tree_mode_t type=IGRAPH_TREE_OUT;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "children",required_argument,0,2 },
                                   { "type",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* children */
      shell_read_integer(&children, optarg);
      break;
    case 3: /* type */
      shell_read_enum(&type, optarg, "out", 0, "in", 1, "undirected", 2, 0);
      break;
    case 4:
      shell_igraph_tree_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_tree(&graph, n, children, type);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_full                                /
/-------------------------------------------*/
void shell_igraph_full_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --directed=<directed> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_full(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_bool_t directed=0;
  igraph_bool_t loops=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "directed",required_argument,0,2 },
                                   { "loops",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 3: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 4:
      shell_igraph_full_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_full(&graph, n, directed, loops);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_full_citation                       /
/-------------------------------------------*/
void shell_igraph_full_citation_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_full_citation(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "directed",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 3:
      shell_igraph_full_citation_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_full_citation(&graph, n, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_atlas                               /
/-------------------------------------------*/
void shell_igraph_atlas_usage(char **argv) {
  printf("%s --graph=<graph> --number=<number>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_atlas(int argc, char **argv) {

  igraph_t graph;
  int number=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "number",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));
  shell_seen[1]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* number */
      shell_read_int(&number, optarg);
      break;
    case 2:
      shell_igraph_atlas_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_atlas(&graph, number);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_extended_chordal_ring               /
/-------------------------------------------*/
void shell_igraph_extended_chordal_ring_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --W=<W>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_extended_chordal_ring(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_matrix_t W;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "W",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* W */
      shell_read_matrix(&W, optarg);
      break;
    case 3:
      shell_igraph_extended_chordal_ring_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_extended_chordal_ring(&graph, nodes, &W);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_matrix_destroy(&W);

  return 0;
}

/*-------------------------------------------/
/ igraph_connect_neighborhood                /
/-------------------------------------------*/
void shell_igraph_connect_neighborhood_usage(char **argv) {
  printf("%s --graph=<graph> --graph-out=<graph-out> --order=<order> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_connect_neighborhood(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t order=2;
  igraph_neimode_t mode=IGRAPH_ALL;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "graph-out",required_argument,0,1 },
                                   { "order",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* graph-out */
      shell_arg_graph=strdup(optarg);
      break;
    case 2: /* order */
      shell_read_integer(&order, optarg);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4:
      shell_igraph_connect_neighborhood_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_connect_neighborhood(&graph, order, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_linegraph                           /
/-------------------------------------------*/
void shell_igraph_linegraph_usage(char **argv) {
  printf("%s --graph=<graph> --linegraph=<linegraph>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_linegraph(int argc, char **argv) {

  igraph_t graph;
  igraph_t linegraph;
  char* shell_arg_linegraph=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "linegraph",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* linegraph */
      shell_arg_linegraph=strdup(optarg);
      break;
    case 2:
      shell_igraph_linegraph_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_linegraph(&graph, &linegraph);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&linegraph, shell_arg_linegraph); 
  igraph_destroy(&linegraph);

  return 0;
}

/*-------------------------------------------/
/ igraph_de_bruijn                           /
/-------------------------------------------*/
void shell_igraph_de_bruijn_usage(char **argv) {
  printf("%s --graph=<graph> --m=<m> --n=<n>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_de_bruijn(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t m;
  igraph_integer_t n;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "m",required_argument,0,1 },
                                   { "n",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* m */
      shell_read_integer(&m, optarg);
      break;
    case 2: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 3:
      shell_igraph_de_bruijn_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_de_bruijn(&graph, m, n);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_kautz                               /
/-------------------------------------------*/
void shell_igraph_kautz_usage(char **argv) {
  printf("%s --graph=<graph> --m=<m> --n=<n>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_kautz(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t m;
  igraph_integer_t n;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "m",required_argument,0,1 },
                                   { "n",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* m */
      shell_read_integer(&m, optarg);
      break;
    case 2: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 3:
      shell_igraph_kautz_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_kautz(&graph, m, n);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_famous                              /
/-------------------------------------------*/
void shell_igraph_famous_usage(char **argv) {
  printf("%s --graph=<graph> --name=<name>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_famous(int argc, char **argv) {

  igraph_t graph;
  char * name="";
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "name",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));
  shell_seen[1]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* name */
      name=strdup(optarg);
      break;
    case 2:
      shell_igraph_famous_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_famous(&graph, name);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  free(name);

  return 0;
}

/*-------------------------------------------/
/ igraph_lcf_vector                          /
/-------------------------------------------*/
void shell_igraph_lcf_vector_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --shifts=<shifts> --repeats=<repeats>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_lcf_vector(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_vector_t shifts;
  igraph_integer_t repeats=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "shifts",required_argument,0,2 },
                                   { "repeats",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* shifts */
      shell_read_vector(&shifts, optarg);
      break;
    case 3: /* repeats */
      shell_read_integer(&repeats, optarg);
      break;
    case 4:
      shell_igraph_lcf_vector_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_lcf_vector(&graph, n, &shifts, repeats);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&shifts);

  return 0;
}

/*-------------------------------------------/
/ igraph_adjlist                             /
/-------------------------------------------*/
void shell_igraph_adjlist_usage(char **argv) {
  printf("%s --graph=<graph> --directed=<directed> --duplicate=<duplicate>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_adjlist(int argc, char **argv) {

  igraph_t graph;

  igraph_bool_t directed=1;
  igraph_bool_t duplicate=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "directed",required_argument,0,1 },
                                   { "duplicate",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[1]=2;
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 2: /* duplicate */
      shell_read_boolean(&duplicate, optarg);
      break;
    case 3:
      shell_igraph_adjlist_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_adjlist(&graph, adjlist, directed, duplicate);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_full_bipartite                      /
/-------------------------------------------*/
void shell_igraph_full_bipartite_usage(char **argv) {
  printf("%s --graph=<graph> --n1=<n1> --n2=<n2> --directed=<directed> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_full_bipartite(int argc, char **argv) {

  igraph_t graph;

  igraph_integer_t n1;
  igraph_integer_t n2;
  igraph_bool_t directed=0;
  igraph_neimode_t mode=IGRAPH_ALL;
  char* shell_arg_graph=0;
  char* shell_arg_types=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n1",required_argument,0,1 },
                                   { "n2",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "mode",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n1 */
      shell_read_integer(&n1, optarg);
      break;
    case 2: /* n2 */
      shell_read_integer(&n2, optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 5:
      shell_igraph_full_bipartite_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_full_bipartite(&graph, types, n1, n2, directed, mode);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_barabasi_game                       /
/-------------------------------------------*/
void shell_igraph_barabasi_game_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --m=<m> --outseq=<outseq> --outpref=<outpref> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_barabasi_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_integer_t m=1;
  igraph_vector_t v_outseq; igraph_vector_t *outseq=0;
  igraph_bool_t outpref=0;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "m",required_argument,0,2 },
                                   { "outseq",required_argument,0,3 },
                                   { "outpref",required_argument,0,4 },
                                   { "directed",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[2]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* m */
      shell_read_integer(&m, optarg);
      break;
    case 3: /* outseq */
      outseq=&v_outseq; shell_read_vector(outseq, optarg);
      break;
    case 4: /* outpref */
      shell_read_boolean(&outpref, optarg);
      break;
    case 5: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 6:
      shell_igraph_barabasi_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_barabasi_game(&graph, n, m, outseq, outpref, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  if (outseq) { igraph_vector_destroy(outseq); }

  return 0;
}

/*-------------------------------------------/
/ igraph_nonlinear_barabasi_game             /
/-------------------------------------------*/
void shell_igraph_nonlinear_barabasi_game_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --power=<power> --m=<m> --outseq=<outseq> --outpref=<outpref> --zeroappeal=<zeroappeal> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_nonlinear_barabasi_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_real_t power=1.0;
  igraph_integer_t m=1;
  igraph_vector_t v_outseq; igraph_vector_t *outseq=0;
  igraph_bool_t outpref=0;
  igraph_real_t zeroappeal=1.0;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "power",required_argument,0,2 },
                                   { "m",required_argument,0,3 },
                                   { "outseq",required_argument,0,4 },
                                   { "outpref",required_argument,0,5 },
                                   { "zeroappeal",required_argument,0,6 },
                                   { "directed",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  shell_seen[7]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* power */
      shell_read_real(&power, optarg);
      break;
    case 3: /* m */
      shell_read_integer(&m, optarg);
      break;
    case 4: /* outseq */
      outseq=&v_outseq; shell_read_vector(outseq, optarg);
      break;
    case 5: /* outpref */
      shell_read_boolean(&outpref, optarg);
      break;
    case 6: /* zeroappeal */
      shell_read_real(&zeroappeal, optarg);
      break;
    case 7: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 8:
      shell_igraph_nonlinear_barabasi_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_nonlinear_barabasi_game(&graph, n, power, m, outseq, outpref, zeroappeal, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  if (outseq) { igraph_vector_destroy(outseq); }

  return 0;
}

/*-------------------------------------------/
/ igraph_erdos_renyi_game_gnp                /
/-------------------------------------------*/
void shell_igraph_erdos_renyi_game_gnp_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --p=<p> --directed=<directed> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_erdos_renyi_game_gnp(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_real_t p;
  igraph_bool_t directed=0;
  igraph_bool_t loops=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "p",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "loops",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* p */
      shell_read_real(&p, optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 5:
      shell_igraph_erdos_renyi_game_gnp_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_erdos_renyi_game_gnp(&graph, n, p, directed, loops);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_erdos_renyi_game_gnm                /
/-------------------------------------------*/
void shell_igraph_erdos_renyi_game_gnm_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --m=<m> --directed=<directed> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_erdos_renyi_game_gnm(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_real_t m;
  igraph_bool_t directed=0;
  igraph_bool_t loops=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "m",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "loops",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* m */
      shell_read_real(&m, optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 5:
      shell_igraph_erdos_renyi_game_gnm_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_erdos_renyi_game_gnm(&graph, n, m, directed, loops);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_degree_sequence_game                /
/-------------------------------------------*/
void shell_igraph_degree_sequence_game_usage(char **argv) {
  printf("%s --graph=<graph> --out_deg=<out_deg> --in_deg=<in_deg> --method=<method>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_degree_sequence_game(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t out_deg;
  igraph_vector_t v_in_deg; igraph_vector_t *in_deg=0;
  igraph_degseq_t method=IGRAPH_DEGSEQ_SIMPLE;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "out_deg",required_argument,0,1 },
                                   { "in_deg",required_argument,0,2 },
                                   { "method",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* out_deg */
      shell_read_vector(&out_deg, optarg);
      break;
    case 2: /* in_deg */
      in_deg=&v_in_deg; shell_read_vector(in_deg, optarg);
      break;
    case 3: /* method */
      shell_read_enum(&method, optarg, "simple", 0, 0);
      break;
    case 4:
      shell_igraph_degree_sequence_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_degree_sequence_game(&graph, &out_deg, in_deg, method);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&out_deg);
  if (in_deg) { igraph_vector_destroy(in_deg); }

  return 0;
}

/*-------------------------------------------/
/ igraph_growing_random_game                 /
/-------------------------------------------*/
void shell_igraph_growing_random_game_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --m=<m> --directed=<directed> --citation=<citation>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_growing_random_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_integer_t m=1;
  igraph_bool_t directed=0;
  igraph_bool_t citation=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "m",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "citation",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* m */
      shell_read_integer(&m, optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4: /* citation */
      shell_read_boolean(&citation, optarg);
      break;
    case 5:
      shell_igraph_growing_random_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_growing_random_game(&graph, n, m, directed, citation);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_barabasi_aging_game                 /
/-------------------------------------------*/
void shell_igraph_barabasi_aging_game_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --m=<m> --outseq=<outseq> --outpref=<outpref> --pa_exp=<pa_exp> --aging_exp=<aging_exp> --aging_bin=<aging_bin> --zero_deg_appeal=<zero_deg_appeal> --zero_age_appeal=<zero_age_appeal> --deg_coef=<deg_coef> --age_coef=<age_coef> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_barabasi_aging_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_integer_t m=1;
  igraph_vector_t v_outseq; igraph_vector_t *outseq=0;
  igraph_bool_t outpref=0;
  igraph_real_t pa_exp=1.0;
  igraph_real_t aging_exp=0.0;
  igraph_integer_t aging_bin=1;
  igraph_real_t zero_deg_appeal=1.0;
  igraph_real_t zero_age_appeal=0.0;
  igraph_real_t deg_coef=1.0;
  igraph_real_t age_coef=1.0;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[13];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "m",required_argument,0,2 },
                                   { "outseq",required_argument,0,3 },
                                   { "outpref",required_argument,0,4 },
                                   { "pa_exp",required_argument,0,5 },
                                   { "aging_exp",required_argument,0,6 },
                                   { "aging_bin",required_argument,0,7 },
                                   { "zero_deg_appeal",required_argument,0,8 },
                                   { "zero_age_appeal",required_argument,0,9 },
                                   { "deg_coef",required_argument,0,10 },
                                   { "age_coef",required_argument,0,11 },
                                   { "directed",required_argument,0,12 },
                                   { "help",no_argument,0,13 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 13*sizeof(int));
  shell_seen[2]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  shell_seen[7]=2;
  shell_seen[8]=2;
  shell_seen[9]=2;
  shell_seen[10]=2;
  shell_seen[11]=2;
  shell_seen[12]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* m */
      shell_read_integer(&m, optarg);
      break;
    case 3: /* outseq */
      outseq=&v_outseq; shell_read_vector(outseq, optarg);
      break;
    case 4: /* outpref */
      shell_read_boolean(&outpref, optarg);
      break;
    case 5: /* pa_exp */
      shell_read_real(&pa_exp, optarg);
      break;
    case 6: /* aging_exp */
      shell_read_real(&aging_exp, optarg);
      break;
    case 7: /* aging_bin */
      shell_read_integer(&aging_bin, optarg);
      break;
    case 8: /* zero_deg_appeal */
      shell_read_real(&zero_deg_appeal, optarg);
      break;
    case 9: /* zero_age_appeal */
      shell_read_real(&zero_age_appeal, optarg);
      break;
    case 10: /* deg_coef */
      shell_read_real(&deg_coef, optarg);
      break;
    case 11: /* age_coef */
      shell_read_real(&age_coef, optarg);
      break;
    case 12: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 13:
      shell_igraph_barabasi_aging_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<13; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_barabasi_aging_game(&graph, nodes, m, outseq, outpref, pa_exp, aging_exp, aging_bin, zero_deg_appeal, zero_age_appeal, deg_coef, age_coef, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  if (outseq) { igraph_vector_destroy(outseq); }

  return 0;
}

/*-------------------------------------------/
/ igraph_recent_degree_game                  /
/-------------------------------------------*/
void shell_igraph_recent_degree_game_usage(char **argv) {
  printf("%s --graph=<graph> --n=<n> --power=<power> --window=<window> --m=<m> --outseq=<outseq> --outpref=<outpref> --zero_appeal=<zero_appeal> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_recent_degree_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t n;
  igraph_real_t power=1.0;
  igraph_integer_t window=1;
  igraph_integer_t m=1;
  igraph_vector_t v_outseq; igraph_vector_t *outseq=0;
  igraph_bool_t outpref=0;
  igraph_real_t zero_appeal=1.0;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[9];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "n",required_argument,0,1 },
                                   { "power",required_argument,0,2 },
                                   { "window",required_argument,0,3 },
                                   { "m",required_argument,0,4 },
                                   { "outseq",required_argument,0,5 },
                                   { "outpref",required_argument,0,6 },
                                   { "zero_appeal",required_argument,0,7 },
                                   { "directed",required_argument,0,8 },
                                   { "help",no_argument,0,9 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 9*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[6]=2;
  shell_seen[7]=2;
  shell_seen[8]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 2: /* power */
      shell_read_real(&power, optarg);
      break;
    case 3: /* window */
      shell_read_integer(&window, optarg);
      break;
    case 4: /* m */
      shell_read_integer(&m, optarg);
      break;
    case 5: /* outseq */
      outseq=&v_outseq; shell_read_vector(outseq, optarg);
      break;
    case 6: /* outpref */
      shell_read_boolean(&outpref, optarg);
      break;
    case 7: /* zero_appeal */
      shell_read_real(&zero_appeal, optarg);
      break;
    case 8: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 9:
      shell_igraph_recent_degree_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<9; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_recent_degree_game(&graph, n, power, window, m, outseq, outpref, zero_appeal, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  if (outseq) { igraph_vector_destroy(outseq); }

  return 0;
}

/*-------------------------------------------/
/ igraph_recent_degree_aging_game            /
/-------------------------------------------*/
void shell_igraph_recent_degree_aging_game_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --m=<m> --outseq=<outseq> --outpref=<outpref> --pa_exp=<pa_exp> --aging_exp=<aging_exp> --aging_bin=<aging_bin> --window=<window> --zero_appeal=<zero_appeal> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_recent_degree_aging_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_integer_t m=1;
  igraph_vector_t v_outseq; igraph_vector_t *outseq=0;
  igraph_bool_t outpref=0;
  igraph_real_t pa_exp=1.0;
  igraph_real_t aging_exp=0.0;
  igraph_integer_t aging_bin=1;
  igraph_integer_t window=1;
  igraph_real_t zero_appeal=1.0;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[11];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "m",required_argument,0,2 },
                                   { "outseq",required_argument,0,3 },
                                   { "outpref",required_argument,0,4 },
                                   { "pa_exp",required_argument,0,5 },
                                   { "aging_exp",required_argument,0,6 },
                                   { "aging_bin",required_argument,0,7 },
                                   { "window",required_argument,0,8 },
                                   { "zero_appeal",required_argument,0,9 },
                                   { "directed",required_argument,0,10 },
                                   { "help",no_argument,0,11 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 11*sizeof(int));
  shell_seen[2]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  shell_seen[7]=2;
  shell_seen[8]=2;
  shell_seen[9]=2;
  shell_seen[10]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* m */
      shell_read_integer(&m, optarg);
      break;
    case 3: /* outseq */
      outseq=&v_outseq; shell_read_vector(outseq, optarg);
      break;
    case 4: /* outpref */
      shell_read_boolean(&outpref, optarg);
      break;
    case 5: /* pa_exp */
      shell_read_real(&pa_exp, optarg);
      break;
    case 6: /* aging_exp */
      shell_read_real(&aging_exp, optarg);
      break;
    case 7: /* aging_bin */
      shell_read_integer(&aging_bin, optarg);
      break;
    case 8: /* window */
      shell_read_integer(&window, optarg);
      break;
    case 9: /* zero_appeal */
      shell_read_real(&zero_appeal, optarg);
      break;
    case 10: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 11:
      shell_igraph_recent_degree_aging_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<11; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_recent_degree_aging_game(&graph, nodes, m, outseq, outpref, pa_exp, aging_exp, aging_bin, window, zero_appeal, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  if (outseq) { igraph_vector_destroy(outseq); }

  return 0;
}

/*-------------------------------------------/
/ igraph_callaway_traits_game                /
/-------------------------------------------*/
void shell_igraph_callaway_traits_game_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --types=<types> --edges_per_step=<edges_per_step> --type_dist=<type_dist> --pref_matrix=<pref_matrix> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_callaway_traits_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_integer_t types;
  igraph_integer_t edges_per_step=1;
  igraph_vector_t type_dist;
  igraph_matrix_t pref_matrix;
  igraph_bool_t directed=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[7];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "types",required_argument,0,2 },
                                   { "edges_per_step",required_argument,0,3 },
                                   { "type_dist",required_argument,0,4 },
                                   { "pref_matrix",required_argument,0,5 },
                                   { "directed",required_argument,0,6 },
                                   { "help",no_argument,0,7 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 7*sizeof(int));
  shell_seen[3]=2;
  shell_seen[6]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* types */
      shell_read_integer(&types, optarg);
      break;
    case 3: /* edges_per_step */
      shell_read_integer(&edges_per_step, optarg);
      break;
    case 4: /* type_dist */
      shell_read_vector(&type_dist, optarg);
      break;
    case 5: /* pref_matrix */
      shell_read_matrix(&pref_matrix, optarg);
      break;
    case 6: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 7:
      shell_igraph_callaway_traits_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<7; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_callaway_traits_game(&graph, nodes, types, edges_per_step, &type_dist, &pref_matrix, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&type_dist);
  igraph_matrix_destroy(&pref_matrix);

  return 0;
}

/*-------------------------------------------/
/ igraph_establishment_game                  /
/-------------------------------------------*/
void shell_igraph_establishment_game_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --types=<types> --k=<k> --type_dist=<type_dist> --pref_matrix=<pref_matrix> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_establishment_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_integer_t types;
  igraph_integer_t k=1;
  igraph_vector_t type_dist;
  igraph_matrix_t pref_matrix;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[7];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "types",required_argument,0,2 },
                                   { "k",required_argument,0,3 },
                                   { "type_dist",required_argument,0,4 },
                                   { "pref_matrix",required_argument,0,5 },
                                   { "directed",required_argument,0,6 },
                                   { "help",no_argument,0,7 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 7*sizeof(int));
  shell_seen[3]=2;
  shell_seen[6]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* types */
      shell_read_integer(&types, optarg);
      break;
    case 3: /* k */
      shell_read_integer(&k, optarg);
      break;
    case 4: /* type_dist */
      shell_read_vector(&type_dist, optarg);
      break;
    case 5: /* pref_matrix */
      shell_read_matrix(&pref_matrix, optarg);
      break;
    case 6: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 7:
      shell_igraph_establishment_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<7; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_establishment_game(&graph, nodes, types, k, &type_dist, &pref_matrix, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&type_dist);
  igraph_matrix_destroy(&pref_matrix);

  return 0;
}

/*-------------------------------------------/
/ igraph_grg_game                            /
/-------------------------------------------*/
void shell_igraph_grg_game_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --radius=<radius> --torus=<torus> --x=<x> --y=<y>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_grg_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_real_t radius;
  igraph_bool_t torus=0;
  igraph_vector_t v_x; igraph_vector_t *x=0;
  igraph_vector_t v_y; igraph_vector_t *y=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "radius",required_argument,0,2 },
                                   { "torus",required_argument,0,3 },
                                   { "x",required_argument,0,4 },
                                   { "y",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* radius */
      shell_read_real(&radius, optarg);
      break;
    case 3: /* torus */
      shell_read_boolean(&torus, optarg);
      break;
    case 4: /* x */
      x=&v_x; shell_read_vector(x, optarg);
      break;
    case 5: /* y */
      y=&v_y; shell_read_vector(y, optarg);
      break;
    case 6:
      shell_igraph_grg_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_grg_game(&graph, nodes, radius, torus, x, y);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  if (x) { igraph_vector_destroy(x); }
  if (y) { igraph_vector_destroy(y); }

  return 0;
}

/*-------------------------------------------/
/ igraph_preference_game                     /
/-------------------------------------------*/
void shell_igraph_preference_game_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --types=<types> --type_dist=<type_dist> --pref_matrix=<pref_matrix> --node_type_vec=<node_type_vec> --directed=<directed> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_preference_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_integer_t types;
  igraph_vector_t type_dist;
  igraph_matrix_t pref_matrix;
  igraph_vector_t node_type_vec;
  igraph_bool_t directed=0;
  igraph_bool_t loops=0;
  char* shell_arg_graph=0;
  char* shell_arg_node_type_vec=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "types",required_argument,0,2 },
                                   { "type_dist",required_argument,0,3 },
                                   { "pref_matrix",required_argument,0,4 },
                                   { "node_type_vec",required_argument,0,5 },
                                   { "directed",required_argument,0,6 },
                                   { "loops",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[6]=2;
  shell_seen[7]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* types */
      shell_read_integer(&types, optarg);
      break;
    case 3: /* type_dist */
      shell_read_vector(&type_dist, optarg);
      break;
    case 4: /* pref_matrix */
      shell_read_matrix(&pref_matrix, optarg);
      break;
    case 5: /* node_type_vec */
      shell_arg_node_type_vec=strdup(optarg); 
  igraph_vector_init(&node_type_vec, 0);
      break;
    case 6: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 7: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 8:
      shell_igraph_preference_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_preference_game(&graph, nodes, types, &type_dist, &pref_matrix, &node_type_vec, directed, loops);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&type_dist);
  igraph_matrix_destroy(&pref_matrix);
  shell_write_vector(&node_type_vec, shell_arg_node_type_vec); 
  igraph_vector_destroy(&node_type_vec);

  return 0;
}

/*-------------------------------------------/
/ igraph_asymmetric_preference_game          /
/-------------------------------------------*/
void shell_igraph_asymmetric_preference_game_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --types=<types> --type_dist_matrix=<type_dist_matrix> --pref_matrix=<pref_matrix> --node_type_in_vec=<node_type_in_vec> --node_type_out_vec=<node_type_out_vec> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_asymmetric_preference_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_integer_t types;
  igraph_matrix_t type_dist_matrix;
  igraph_matrix_t pref_matrix;
  igraph_vector_t node_type_in_vec;
  igraph_vector_t node_type_out_vec;
  igraph_bool_t loops=0;
  char* shell_arg_graph=0;
  char* shell_arg_node_type_in_vec=0;
  char* shell_arg_node_type_out_vec=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "types",required_argument,0,2 },
                                   { "type_dist_matrix",required_argument,0,3 },
                                   { "pref_matrix",required_argument,0,4 },
                                   { "node_type_in_vec",required_argument,0,5 },
                                   { "node_type_out_vec",required_argument,0,6 },
                                   { "loops",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[7]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* types */
      shell_read_integer(&types, optarg);
      break;
    case 3: /* type_dist_matrix */
      shell_read_matrix(&type_dist_matrix, optarg);
      break;
    case 4: /* pref_matrix */
      shell_read_matrix(&pref_matrix, optarg);
      break;
    case 5: /* node_type_in_vec */
      shell_arg_node_type_in_vec=strdup(optarg); 
  igraph_vector_init(&node_type_in_vec, 0);
      break;
    case 6: /* node_type_out_vec */
      shell_arg_node_type_out_vec=strdup(optarg); 
  igraph_vector_init(&node_type_out_vec, 0);
      break;
    case 7: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 8:
      shell_igraph_asymmetric_preference_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_asymmetric_preference_game(&graph, nodes, types, &type_dist_matrix, &pref_matrix, &node_type_in_vec, &node_type_out_vec, loops);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_matrix_destroy(&type_dist_matrix);
  igraph_matrix_destroy(&pref_matrix);
  shell_write_vector(&node_type_in_vec, shell_arg_node_type_in_vec); 
  igraph_vector_destroy(&node_type_in_vec);
  shell_write_vector(&node_type_out_vec, shell_arg_node_type_out_vec); 
  igraph_vector_destroy(&node_type_out_vec);

  return 0;
}

/*-------------------------------------------/
/ igraph_rewire_edges                        /
/-------------------------------------------*/
void shell_igraph_rewire_edges_usage(char **argv) {
  printf("%s --graph=<graph> --graph-out=<graph-out> --prob=<prob>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_rewire_edges(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t prob;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "graph-out",required_argument,0,1 },
                                   { "prob",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* graph-out */
      shell_arg_graph=strdup(optarg);
      break;
    case 2: /* prob */
      shell_read_real(&prob, optarg);
      break;
    case 3:
      shell_igraph_rewire_edges_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_rewire_edges(&graph, prob);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_watts_strogatz_game                 /
/-------------------------------------------*/
void shell_igraph_watts_strogatz_game_usage(char **argv) {
  printf("%s --graph=<graph> --dim=<dim> --size=<size> --nei=<nei> --p=<p>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_watts_strogatz_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t dim;
  igraph_integer_t size;
  igraph_integer_t nei;
  igraph_real_t p;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "dim",required_argument,0,1 },
                                   { "size",required_argument,0,2 },
                                   { "nei",required_argument,0,3 },
                                   { "p",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* dim */
      shell_read_integer(&dim, optarg);
      break;
    case 2: /* size */
      shell_read_integer(&size, optarg);
      break;
    case 3: /* nei */
      shell_read_integer(&nei, optarg);
      break;
    case 4: /* p */
      shell_read_real(&p, optarg);
      break;
    case 5:
      shell_igraph_watts_strogatz_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_watts_strogatz_game(&graph, dim, size, nei, p);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_lastcit_game                        /
/-------------------------------------------*/
void shell_igraph_lastcit_game_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --edges_per_node=<edges_per_node> --agebins=<agebins> --preference=<preference> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_lastcit_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_integer_t edges_per_node=1;
  igraph_integer_t agebins=1;
  igraph_vector_t preference;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "edges_per_node",required_argument,0,2 },
                                   { "agebins",required_argument,0,3 },
                                   { "preference",required_argument,0,4 },
                                   { "directed",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* edges_per_node */
      shell_read_integer(&edges_per_node, optarg);
      break;
    case 3: /* agebins */
      shell_read_integer(&agebins, optarg);
      break;
    case 4: /* preference */
      shell_read_vector(&preference, optarg);
      break;
    case 5: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 6:
      shell_igraph_lastcit_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_lastcit_game(&graph, nodes, edges_per_node, agebins, &preference, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&preference);

  return 0;
}

/*-------------------------------------------/
/ igraph_cited_type_game                     /
/-------------------------------------------*/
void shell_igraph_cited_type_game_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --types=<types> --pref=<pref> --edges_per_step=<edges_per_step> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_cited_type_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_vector_t types;
  igraph_vector_t pref;
  igraph_integer_t edges_per_step=1;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "types",required_argument,0,2 },
                                   { "pref",required_argument,0,3 },
                                   { "edges_per_step",required_argument,0,4 },
                                   { "directed",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* types */
      shell_read_vector(&types, optarg);
      break;
    case 3: /* pref */
      shell_read_vector(&pref, optarg);
      break;
    case 4: /* edges_per_step */
      shell_read_integer(&edges_per_step, optarg);
      break;
    case 5: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 6:
      shell_igraph_cited_type_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_cited_type_game(&graph, nodes, &types, &pref, edges_per_step, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&types);
  igraph_vector_destroy(&pref);

  return 0;
}

/*-------------------------------------------/
/ igraph_citing_cited_type_game              /
/-------------------------------------------*/
void shell_igraph_citing_cited_type_game_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --types=<types> --pref=<pref> --edges_per_step=<edges_per_step> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_citing_cited_type_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_vector_t types;
  igraph_matrix_t pref;
  igraph_integer_t edges_per_step=1;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "types",required_argument,0,2 },
                                   { "pref",required_argument,0,3 },
                                   { "edges_per_step",required_argument,0,4 },
                                   { "directed",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* types */
      shell_read_vector(&types, optarg);
      break;
    case 3: /* pref */
      shell_read_matrix(&pref, optarg);
      break;
    case 4: /* edges_per_step */
      shell_read_integer(&edges_per_step, optarg);
      break;
    case 5: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 6:
      shell_igraph_citing_cited_type_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_citing_cited_type_game(&graph, nodes, &types, &pref, edges_per_step, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&types);
  igraph_matrix_destroy(&pref);

  return 0;
}

/*-------------------------------------------/
/ igraph_forest_fire_game                    /
/-------------------------------------------*/
void shell_igraph_forest_fire_game_usage(char **argv) {
  printf("%s --graph=<graph> --nodes=<nodes> --fw_prob=<fw_prob> --bw_factor=<bw_factor> --ambs=<ambs> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_forest_fire_game(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t nodes;
  igraph_real_t fw_prob;
  igraph_real_t bw_factor=1;
  igraph_integer_t ambs=1;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "nodes",required_argument,0,1 },
                                   { "fw_prob",required_argument,0,2 },
                                   { "bw_factor",required_argument,0,3 },
                                   { "ambs",required_argument,0,4 },
                                   { "directed",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* nodes */
      shell_read_integer(&nodes, optarg);
      break;
    case 2: /* fw_prob */
      shell_read_real(&fw_prob, optarg);
      break;
    case 3: /* bw_factor */
      shell_read_real(&bw_factor, optarg);
      break;
    case 4: /* ambs */
      shell_read_integer(&ambs, optarg);
      break;
    case 5: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 6:
      shell_igraph_forest_fire_game_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_forest_fire_game(&graph, nodes, fw_prob, bw_factor, ambs, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_are_connected                       /
/-------------------------------------------*/
void shell_igraph_are_connected_usage(char **argv) {
  printf("%s --graph=<graph> --v1=<v1> --v2=<v2> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_are_connected(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t v1;
  igraph_integer_t v2;
  igraph_bool_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "v1",required_argument,0,1 },
                                   { "v2",required_argument,0,2 },
                                   { "res",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* v1 */
      shell_read_integer(&v1, optarg);
      break;
    case 2: /* v2 */
      shell_read_integer(&v2, optarg);
      break;
    case 3: /* res */
      
      break;
    case 4:
      shell_igraph_are_connected_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_are_connected(&graph, v1, v2, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_boolean(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_diameter                            /
/-------------------------------------------*/
void shell_igraph_diameter_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --from=<from> --to=<to> --path=<path> --directed=<directed> --unconnected=<unconnected>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_diameter(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t res;
  igraph_integer_t from;
  igraph_integer_t to;
  igraph_vector_t v_path; igraph_vector_t *path=0;
  igraph_bool_t directed=1;
  igraph_bool_t unconnected=1;
  char* shell_arg_res=0;
  char* shell_arg_from=0;
  char* shell_arg_to=0;
  char* shell_arg_path=0;
  int shell_result;


  int shell_seen[7];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "from",required_argument,0,2 },
                                   { "to",required_argument,0,3 },
                                   { "path",required_argument,0,4 },
                                   { "directed",required_argument,0,5 },
                                   { "unconnected",required_argument,0,6 },
                                   { "help",no_argument,0,7 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 7*sizeof(int));
  shell_seen[5]=2;
  shell_seen[6]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* from */
      shell_arg_from=strdup(optarg);
      break;
    case 3: /* to */
      shell_arg_to=strdup(optarg);
      break;
    case 4: /* path */
      path=&v_path; igraph_vector_init(path, 0); 
  shell_arg_path=strdup(optarg);
      break;
    case 5: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 6: /* unconnected */
      shell_read_boolean(&unconnected, optarg);
      break;
    case 7:
      shell_igraph_diameter_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<7; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_diameter(&graph, &res, &from, &to, path, directed, unconnected);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(res, shell_arg_res);
  shell_write_integer(from, shell_arg_from);
  shell_write_integer(to, shell_arg_to);
  if (path) { shell_write_vector(path, shell_arg_path); 
  igraph_vector_destroy(path); }

  return 0;
}

/*-------------------------------------------/
/ igraph_minimum_spanning_tree_unweighted    /
/-------------------------------------------*/
void shell_igraph_minimum_spanning_tree_unweighted_usage(char **argv) {
  printf("%s --graph=<graph> --mst=<mst>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_minimum_spanning_tree_unweighted(int argc, char **argv) {

  igraph_t graph;
  igraph_t mst;
  char* shell_arg_mst=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "mst",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* mst */
      shell_arg_mst=strdup(optarg);
      break;
    case 2:
      shell_igraph_minimum_spanning_tree_unweighted_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_minimum_spanning_tree_unweighted(&graph, &mst);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&mst, shell_arg_mst); 
  igraph_destroy(&mst);

  return 0;
}

/*-------------------------------------------/
/ igraph_minimum_spanning_tree_prim          /
/-------------------------------------------*/
void shell_igraph_minimum_spanning_tree_prim_usage(char **argv) {
  printf("%s --graph=<graph> --mst=<mst> --weights=<weights>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_minimum_spanning_tree_prim(int argc, char **argv) {

  igraph_t graph;
  igraph_t mst;
  igraph_vector_t weights;
  char* shell_arg_mst=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "mst",required_argument,0,1 },
                                   { "weights",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* mst */
      shell_arg_mst=strdup(optarg);
      break;
    case 2: /* weights */
      shell_read_vector(&weights, optarg);
      break;
    case 3:
      shell_igraph_minimum_spanning_tree_prim_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_minimum_spanning_tree_prim(&graph, &mst, &weights);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&mst, shell_arg_mst); 
  igraph_destroy(&mst);
  igraph_vector_destroy(&weights);

  return 0;
}

/*-------------------------------------------/
/ igraph_closeness                           /
/-------------------------------------------*/
void shell_igraph_closeness_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_closeness(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_neimode_t mode=IGRAPH_OUT;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4:
      shell_igraph_closeness_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_closeness(&graph, &res, vids, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_closeness_estimate                  /
/-------------------------------------------*/
void shell_igraph_closeness_estimate_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --mode=<mode> --cutoff=<cutoff>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_closeness_estimate(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_neimode_t mode=IGRAPH_OUT;
  igraph_integer_t cutoff;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "cutoff",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4: /* cutoff */
      shell_read_integer(&cutoff, optarg);
      break;
    case 5:
      shell_igraph_closeness_estimate_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_closeness_estimate(&graph, &res, vids, mode, cutoff);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_shortest_paths                      /
/-------------------------------------------*/
void shell_igraph_shortest_paths_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --from=<from> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_shortest_paths(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_vector_t v_from; igraph_vs_t from=igraph_vss_all();
  igraph_neimode_t mode=IGRAPH_OUT;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "from",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* from */
      shell_read_vector(&v_from, optarg); igraph_vs_vector(&from, &v_from);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4:
      shell_igraph_shortest_paths_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_shortest_paths(&graph, &res, from, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (!igraph_vs_is_all(&from)) { igraph_vector_destroy(&v_from); }

  return 0;
}

/*-------------------------------------------/
/ igraph_get_shortest_paths                  /
/-------------------------------------------*/
void shell_igraph_get_shortest_paths_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --from=<from> --to=<to> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_get_shortest_paths(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_ptr_t res;
  igraph_integer_t from;
  igraph_vector_t v_to; igraph_vs_t to=igraph_vss_all();
  igraph_neimode_t mode=IGRAPH_OUT;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "from",required_argument,0,2 },
                                   { "to",required_argument,0,3 },
                                   { "mode",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_ptr_init(&res, 0);
      break;
    case 2: /* from */
      shell_read_integer(&from, optarg);
      break;
    case 3: /* to */
      shell_read_vector(&v_to, optarg); igraph_vs_vector(&to, &v_to);
      break;
    case 4: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 5:
      shell_igraph_get_shortest_paths_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_get_shortest_paths(&graph, &res, from, to, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vectorlist(&res, shell_arg_res); 
  shell_free_vectorlist(&res); 
  igraph_vector_ptr_destroy(&res);
  if (!igraph_vs_is_all(&to)) { igraph_vector_destroy(&v_to); }

  return 0;
}

/*-------------------------------------------/
/ igraph_get_all_shortest_paths              /
/-------------------------------------------*/
void shell_igraph_get_all_shortest_paths_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --nrgeo=<nrgeo> --from=<from> --to=<to> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_get_all_shortest_paths(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_ptr_t res;
  igraph_vector_t nrgeo;
  igraph_integer_t from;
  igraph_vector_t v_to; igraph_vs_t to;
  igraph_neimode_t mode=IGRAPH_OUT;
  char* shell_arg_res=0;
  char* shell_arg_nrgeo=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "nrgeo",required_argument,0,2 },
                                   { "from",required_argument,0,3 },
                                   { "to",required_argument,0,4 },
                                   { "mode",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_ptr_init(&res, 0);
      break;
    case 2: /* nrgeo */
      shell_arg_nrgeo=strdup(optarg); 
  igraph_vector_init(&nrgeo, 0);
      break;
    case 3: /* from */
      shell_read_integer(&from, optarg);
      break;
    case 4: /* to */
      shell_read_vector(&v_to, optarg); igraph_vs_vector(&to, &v_to);
      break;
    case 5: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 6:
      shell_igraph_get_all_shortest_paths_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_get_all_shortest_paths(&graph, &res, &nrgeo, from, to, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vectorlist(&res, shell_arg_res); 
  shell_free_vectorlist(&res); 
  igraph_vector_ptr_destroy(&res);
  shell_write_vector(&nrgeo, shell_arg_nrgeo); 
  igraph_vector_destroy(&nrgeo);
  if (!igraph_vs_is_all(&to)) { igraph_vector_destroy(&v_to); }

  return 0;
}

/*-------------------------------------------/
/ igraph_shortest_paths_dijkstra             /
/-------------------------------------------*/
void shell_igraph_shortest_paths_dijkstra_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --from=<from> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_shortest_paths_dijkstra(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_vector_t v_from; igraph_vs_t from=igraph_vss_all();

  igraph_neimode_t mode=IGRAPH_OUT;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "from",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* from */
      shell_read_vector(&v_from, optarg); igraph_vs_vector(&from, &v_from);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4:
      shell_igraph_shortest_paths_dijkstra_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_shortest_paths_dijkstra(&graph, &res, from, 0, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (!igraph_vs_is_all(&from)) { igraph_vector_destroy(&v_from); }

  return 0;
}

/*-------------------------------------------/
/ igraph_shortest_paths_bellman_ford         /
/-------------------------------------------*/
void shell_igraph_shortest_paths_bellman_ford_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --from=<from> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_shortest_paths_bellman_ford(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_vector_t v_from; igraph_vs_t from=igraph_vss_all();

  igraph_neimode_t mode=IGRAPH_OUT;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "from",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* from */
      shell_read_vector(&v_from, optarg); igraph_vs_vector(&from, &v_from);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4:
      shell_igraph_shortest_paths_bellman_ford_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_shortest_paths_bellman_ford(&graph, &res, from, 0, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (!igraph_vs_is_all(&from)) { igraph_vector_destroy(&v_from); }

  return 0;
}

/*-------------------------------------------/
/ igraph_subcomponent                        /
/-------------------------------------------*/
void shell_igraph_subcomponent_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vid=<vid> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_subcomponent(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_real_t vid;
  igraph_neimode_t mode=IGRAPH_ALL;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vid",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* vid */
      shell_read_real(&vid, optarg);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4:
      shell_igraph_subcomponent_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_subcomponent(&graph, &res, vid, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_betweenness                         /
/-------------------------------------------*/
void shell_igraph_betweenness_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_betweenness(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_bool_t directed=1;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4:
      shell_igraph_betweenness_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_betweenness(&graph, &res, vids, directed);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_betweenness_estimate                /
/-------------------------------------------*/
void shell_igraph_betweenness_estimate_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --directed=<directed> --cutoff=<cutoff>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_betweenness_estimate(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_bool_t directed=1;
  igraph_integer_t cutoff;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "cutoff",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4: /* cutoff */
      shell_read_integer(&cutoff, optarg);
      break;
    case 5:
      shell_igraph_betweenness_estimate_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_betweenness_estimate(&graph, &res, vids, directed, cutoff);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_edge_betweenness                    /
/-------------------------------------------*/
void shell_igraph_edge_betweenness_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_edge_betweenness(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_bool_t directed=1;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "directed",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 3:
      shell_igraph_edge_betweenness_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_edge_betweenness(&graph, &res, directed);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_edge_betweenness_estimate           /
/-------------------------------------------*/
void shell_igraph_edge_betweenness_estimate_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --directed=<directed> --cutoff=<cutoff>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_edge_betweenness_estimate(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_bool_t directed=1;
  igraph_integer_t cutoff;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "directed",required_argument,0,2 },
                                   { "cutoff",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 3: /* cutoff */
      shell_read_integer(&cutoff, optarg);
      break;
    case 4:
      shell_igraph_edge_betweenness_estimate_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_edge_betweenness_estimate(&graph, &res, directed, cutoff);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_pagerank_old                        /
/-------------------------------------------*/
void shell_igraph_pagerank_old_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --directed=<directed> --niter=<niter> --eps=<eps> --damping=<damping> --old=<old>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_pagerank_old(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_bool_t directed=1;
  igraph_integer_t niter=1000;
  igraph_real_t eps=0.001;
  igraph_real_t damping=0.85;
  igraph_bool_t old=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "niter",required_argument,0,4 },
                                   { "eps",required_argument,0,5 },
                                   { "damping",required_argument,0,6 },
                                   { "old",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  shell_seen[7]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 5: /* eps */
      shell_read_real(&eps, optarg);
      break;
    case 6: /* damping */
      shell_read_real(&damping, optarg);
      break;
    case 7: /* old */
      shell_read_boolean(&old, optarg);
      break;
    case 8:
      shell_igraph_pagerank_old_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_pagerank_old(&graph, &res, vids, directed, niter, eps, damping, old);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_pagerank                            /
/-------------------------------------------*/
void shell_igraph_pagerank_usage(char **argv) {
  printf("%s --graph=<graph> --vector=<vector> --value=<value> --vids=<vids> --directed=<directed> --damping=<damping>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_pagerank(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t vector;
  igraph_real_t value;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_bool_t directed=1;
  igraph_real_t damping=0.85;


  char* shell_arg_vector=0;
  char* shell_arg_value=0;
  char* shell_arg_options=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "vector",required_argument,0,1 },
                                   { "value",required_argument,0,2 },
                                   { "vids",required_argument,0,3 },
                                   { "directed",required_argument,0,4 },
                                   { "damping",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* vector */
      shell_arg_vector=strdup(optarg); 
  igraph_vector_init(&vector, 0);
      break;
    case 2: /* value */
      shell_arg_value=strdup(optarg);
      break;
    case 3: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 4: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 5: /* damping */
      shell_read_real(&damping, optarg);
      break;
    case 6:
      shell_igraph_pagerank_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_pagerank(&graph, &vector, &value, vids, directed, damping, 0, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&vector, shell_arg_vector); 
  igraph_vector_destroy(&vector);
  shell_write_real(value, shell_arg_value);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_rewire                              /
/-------------------------------------------*/
void shell_igraph_rewire_usage(char **argv) {
  printf("%s --rewire=<rewire> --rewire-out=<rewire-out> --n=<n> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_rewire(int argc, char **argv) {

  igraph_t rewire;
  igraph_integer_t n;
  igraph_rewiring_t mode=IGRAPH_REWIRING_SIMPLE;
  char* shell_arg_rewire=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "rewire",required_argument,0,0 },
                                   { "rewire-out",required_argument,0,1 },
                                   { "n",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* rewire */
      shell_read_graph(&rewire, optarg);
      break;
    case 1: /* rewire-out */
      shell_arg_rewire=strdup(optarg);
      break;
    case 2: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "simple", 0, 0);
      break;
    case 4:
      shell_igraph_rewire_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_rewire(&rewire, n, mode);

  /* Write the result */
  igraph_destroy(&rewire);
  shell_write_graph(&rewire, shell_arg_rewire); 
  igraph_destroy(&rewire);

  return 0;
}

/*-------------------------------------------/
/ igraph_subgraph                            /
/-------------------------------------------*/
void shell_igraph_subgraph_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_subgraph(int argc, char **argv) {

  igraph_t graph;
  igraph_t res;
  igraph_vector_t v_vids; igraph_vs_t vids;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3:
      shell_igraph_subgraph_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_subgraph(&graph, &res, vids);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&res, shell_arg_res); 
  igraph_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_average_path_length                 /
/-------------------------------------------*/
void shell_igraph_average_path_length_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --directed=<directed> --unconn=<unconn>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_average_path_length(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t res;
  igraph_bool_t directed=1;
  igraph_bool_t unconn=1;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "directed",required_argument,0,2 },
                                   { "unconn",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 3: /* unconn */
      shell_read_boolean(&unconn, optarg);
      break;
    case 4:
      shell_igraph_average_path_length_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_average_path_length(&graph, &res, directed, unconn);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_path_length_hist                    /
/-------------------------------------------*/
void shell_igraph_path_length_hist_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --unconnected=<unconnected> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_path_length_hist(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_real_t unconnected;
  igraph_bool_t directed=1;
  char* shell_arg_res=0;
  char* shell_arg_unconnected=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "unconnected",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* unconnected */
      shell_arg_unconnected=strdup(optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4:
      shell_igraph_path_length_hist_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_path_length_hist(&graph, &res, &unconnected, directed);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  shell_write_real(unconnected, shell_arg_unconnected);

  return 0;
}

/*-------------------------------------------/
/ igraph_simplify                            /
/-------------------------------------------*/
void shell_igraph_simplify_usage(char **argv) {
  printf("%s --graph=<graph> --graph-out=<graph-out> --multiple=<multiple> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_simplify(int argc, char **argv) {

  igraph_t graph;
  igraph_bool_t multiple=1;
  igraph_bool_t loops=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "graph-out",required_argument,0,1 },
                                   { "multiple",required_argument,0,2 },
                                   { "loops",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* graph-out */
      shell_arg_graph=strdup(optarg);
      break;
    case 2: /* multiple */
      shell_read_boolean(&multiple, optarg);
      break;
    case 3: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 4:
      shell_igraph_simplify_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_simplify(&graph, multiple, loops);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_transitivity_undirected             /
/-------------------------------------------*/
void shell_igraph_transitivity_undirected_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_transitivity_undirected(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2:
      shell_igraph_transitivity_undirected_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_transitivity_undirected(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_transitivity_local_undirected       /
/-------------------------------------------*/
void shell_igraph_transitivity_local_undirected_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_transitivity_local_undirected(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3:
      shell_igraph_transitivity_local_undirected_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_transitivity_local_undirected(&graph, &res, vids);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_transitivity_avglocal_undirected    /
/-------------------------------------------*/
void shell_igraph_transitivity_avglocal_undirected_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_transitivity_avglocal_undirected(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2:
      shell_igraph_transitivity_avglocal_undirected_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_transitivity_avglocal_undirected(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_reciprocity                         /
/-------------------------------------------*/
void shell_igraph_reciprocity_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --ignore_loops=<ignore_loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_reciprocity(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t res;
  igraph_bool_t ignore_loops=1;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "ignore_loops",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* ignore_loops */
      shell_read_boolean(&ignore_loops, optarg);
      break;
    case 3:
      shell_igraph_reciprocity_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_reciprocity(&graph, &res, ignore_loops);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_constraint                          /
/-------------------------------------------*/
void shell_igraph_constraint_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --weights=<weights>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_constraint(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_vector_t v_weights; igraph_vector_t *weights=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "weights",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* weights */
      weights=&v_weights; shell_read_vector(weights, optarg);
      break;
    case 4:
      shell_igraph_constraint_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_constraint(&graph, &res, vids, weights);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }
  if (weights) { igraph_vector_destroy(weights); }

  return 0;
}

/*-------------------------------------------/
/ igraph_maxdegree                           /
/-------------------------------------------*/
void shell_igraph_maxdegree_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --mode=<mode> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_maxdegree(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_neimode_t mode=IGRAPH_ALL;
  igraph_bool_t loops=1;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "loops",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 5:
      shell_igraph_maxdegree_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_maxdegree(&graph, &res, vids, mode, loops);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(res, shell_arg_res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_density                             /
/-------------------------------------------*/
void shell_igraph_density_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_density(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t res;
  igraph_bool_t loops=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "loops",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 3:
      shell_igraph_density_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_density(&graph, &res, loops);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_neighborhood_size                   /
/-------------------------------------------*/
void shell_igraph_neighborhood_size_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --order=<order> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_neighborhood_size(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_vector_t v_vids; igraph_vs_t vids;
  igraph_integer_t order;
  igraph_neimode_t mode=IGRAPH_ALL;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "order",required_argument,0,3 },
                                   { "mode",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* order */
      shell_read_integer(&order, optarg);
      break;
    case 4: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 5:
      shell_igraph_neighborhood_size_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_neighborhood_size(&graph, &res, vids, order, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_neighborhood                        /
/-------------------------------------------*/
void shell_igraph_neighborhood_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --order=<order> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_neighborhood(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_ptr_t res;
  igraph_vector_t v_vids; igraph_vs_t vids;
  igraph_integer_t order;
  igraph_neimode_t mode=IGRAPH_ALL;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "order",required_argument,0,3 },
                                   { "mode",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_ptr_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* order */
      shell_read_integer(&order, optarg);
      break;
    case 4: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 5:
      shell_igraph_neighborhood_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_neighborhood(&graph, &res, vids, order, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vectorlist(&res, shell_arg_res); 
  shell_free_vectorlist(&res); 
  igraph_vector_ptr_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_neighborhood_graphs                 /
/-------------------------------------------*/
void shell_igraph_neighborhood_graphs_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --order=<order> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_neighborhood_graphs(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_ptr_t res;
  igraph_vector_t v_vids; igraph_vs_t vids;
  igraph_integer_t order;
  igraph_neimode_t mode=IGRAPH_ALL;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "order",required_argument,0,3 },
                                   { "mode",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_ptr_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* order */
      shell_read_integer(&order, optarg);
      break;
    case 4: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 5:
      shell_igraph_neighborhood_graphs_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_neighborhood_graphs(&graph, &res, vids, order, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graphlist(&res, shell_arg_res); 
  shell_free_graphlist(&res); 
  igraph_vector_ptr_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_topological_sorting                 /
/-------------------------------------------*/
void shell_igraph_topological_sorting_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_topological_sorting(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_neimode_t mode=IGRAPH_OUT;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "mode",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 3:
      shell_igraph_topological_sorting_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_topological_sorting(&graph, &res, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_is_loop                             /
/-------------------------------------------*/
void shell_igraph_is_loop_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --es=<es>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_is_loop(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_bool_t res;
  igraph_vector_t v_es; igraph_es_t es=igraph_ess_all(IGRAPH_EDGEORDER_ID);
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "es",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_bool_init(&res, 0);
      break;
    case 2: /* es */
      shell_read_vector(&v_es, optarg); igraph_es_vector(&es, &v_es);
      break;
    case 3:
      shell_igraph_is_loop_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_is_loop(&graph, &res, es);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector_bool(&res, shell_arg_res); 
  igraph_vector_bool_destroy(&res);
  if (!igraph_es_is_all(&es)) { igraph_es_destroy(&es); }

  return 0;
}

/*-------------------------------------------/
/ igraph_is_simple                           /
/-------------------------------------------*/
void shell_igraph_is_simple_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_is_simple(int argc, char **argv) {

  igraph_t graph;
  igraph_bool_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      
      break;
    case 2:
      shell_igraph_is_simple_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_is_simple(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_boolean(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_is_multiple                         /
/-------------------------------------------*/
void shell_igraph_is_multiple_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --es=<es>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_is_multiple(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_bool_t res;
  igraph_vector_t v_es; igraph_es_t es=igraph_ess_all(IGRAPH_EDGEORDER_ID);
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "es",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_bool_init(&res, 0);
      break;
    case 2: /* es */
      shell_read_vector(&v_es, optarg); igraph_es_vector(&es, &v_es);
      break;
    case 3:
      shell_igraph_is_multiple_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_is_multiple(&graph, &res, es);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector_bool(&res, shell_arg_res); 
  igraph_vector_bool_destroy(&res);
  if (!igraph_es_is_all(&es)) { igraph_es_destroy(&es); }

  return 0;
}

/*-------------------------------------------/
/ igraph_count_multiple                      /
/-------------------------------------------*/
void shell_igraph_count_multiple_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --es=<es>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_count_multiple(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_vector_t v_es; igraph_es_t es=igraph_ess_all(IGRAPH_EDGEORDER_ID);
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "es",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* es */
      shell_read_vector(&v_es, optarg); igraph_es_vector(&es, &v_es);
      break;
    case 3:
      shell_igraph_count_multiple_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_count_multiple(&graph, &res, es);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  if (!igraph_es_is_all(&es)) { igraph_es_destroy(&es); }

  return 0;
}

/*-------------------------------------------/
/ igraph_girth                               /
/-------------------------------------------*/
void shell_igraph_girth_usage(char **argv) {
  printf("%s --graph=<graph> --girth=<girth> --circle=<circle>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_girth(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t girth;
  igraph_vector_t circle;
  char* shell_arg_girth=0;
  char* shell_arg_circle=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "girth",required_argument,0,1 },
                                   { "circle",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* girth */
      shell_arg_girth=strdup(optarg);
      break;
    case 2: /* circle */
      shell_arg_circle=strdup(optarg); 
  igraph_vector_init(&circle, 0);
      break;
    case 3:
      shell_igraph_girth_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_girth(&graph, &girth, &circle);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(girth, shell_arg_girth);
  shell_write_vector(&circle, shell_arg_circle); 
  igraph_vector_destroy(&circle);

  return 0;
}

/*-------------------------------------------/
/ igraph_add_edge                            /
/-------------------------------------------*/
void shell_igraph_add_edge_usage(char **argv) {
  printf("%s --graph=<graph> --graph-out=<graph-out> --from=<from> --to=<to>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_add_edge(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t from;
  igraph_integer_t to;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "graph-out",required_argument,0,1 },
                                   { "from",required_argument,0,2 },
                                   { "to",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* graph-out */
      shell_arg_graph=strdup(optarg);
      break;
    case 2: /* from */
      shell_read_integer(&from, optarg);
      break;
    case 3: /* to */
      shell_read_integer(&to, optarg);
      break;
    case 4:
      shell_igraph_add_edge_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_add_edge(&graph, from, to);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_eigenvector_centrality              /
/-------------------------------------------*/
void shell_igraph_eigenvector_centrality_usage(char **argv) {
  printf("%s --graph=<graph> --vector=<vector> --value=<value> --scale=<scale>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_eigenvector_centrality(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t vector;
  igraph_real_t value;
  igraph_bool_t scale=1;


  char* shell_arg_vector=0;
  char* shell_arg_value=0;
  char* shell_arg_options=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "vector",required_argument,0,1 },
                                   { "value",required_argument,0,2 },
                                   { "scale",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* vector */
      shell_arg_vector=strdup(optarg); 
  igraph_vector_init(&vector, 0);
      break;
    case 2: /* value */
      shell_arg_value=strdup(optarg);
      break;
    case 3: /* scale */
      shell_read_boolean(&scale, optarg);
      break;
    case 4:
      shell_igraph_eigenvector_centrality_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_eigenvector_centrality(&graph, &vector, &value, scale, 0, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&vector, shell_arg_vector); 
  igraph_vector_destroy(&vector);
  shell_write_real(value, shell_arg_value);

  return 0;
}

/*-------------------------------------------/
/ igraph_hub_score                           /
/-------------------------------------------*/
void shell_igraph_hub_score_usage(char **argv) {
  printf("%s --graph=<graph> --vector=<vector> --value=<value> --scale=<scale>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_hub_score(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t vector;
  igraph_real_t value;
  igraph_bool_t scale=1;

  char* shell_arg_vector=0;
  char* shell_arg_value=0;
  char* shell_arg_options=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "vector",required_argument,0,1 },
                                   { "value",required_argument,0,2 },
                                   { "scale",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* vector */
      shell_arg_vector=strdup(optarg); 
  igraph_vector_init(&vector, 0);
      break;
    case 2: /* value */
      shell_arg_value=strdup(optarg);
      break;
    case 3: /* scale */
      shell_read_boolean(&scale, optarg);
      break;
    case 4:
      shell_igraph_hub_score_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_hub_score(&graph, &vector, &value, scale, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&vector, shell_arg_vector); 
  igraph_vector_destroy(&vector);
  shell_write_real(value, shell_arg_value);

  return 0;
}

/*-------------------------------------------/
/ igraph_authority_score                     /
/-------------------------------------------*/
void shell_igraph_authority_score_usage(char **argv) {
  printf("%s --graph=<graph> --vector=<vector> --value=<value> --scale=<scale>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_authority_score(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t vector;
  igraph_real_t value;
  igraph_bool_t scale=1;

  char* shell_arg_vector=0;
  char* shell_arg_value=0;
  char* shell_arg_options=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "vector",required_argument,0,1 },
                                   { "value",required_argument,0,2 },
                                   { "scale",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* vector */
      shell_arg_vector=strdup(optarg); 
  igraph_vector_init(&vector, 0);
      break;
    case 2: /* value */
      shell_arg_value=strdup(optarg);
      break;
    case 3: /* scale */
      shell_read_boolean(&scale, optarg);
      break;
    case 4:
      shell_igraph_authority_score_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_authority_score(&graph, &vector, &value, scale, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&vector, shell_arg_vector); 
  igraph_vector_destroy(&vector);
  shell_write_real(value, shell_arg_value);

  return 0;
}

/*-------------------------------------------/
/ igraph_arpack_rssolve                      /
/-------------------------------------------*/
void shell_igraph_arpack_rssolve_usage(char **argv) {
  printf("%s --values=<values>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_arpack_rssolve(int argc, char **argv) {





  igraph_vector_t v_values; igraph_vector_t *values=0;

  char* shell_arg_options=0;
  char* shell_arg_values=0;
  char* shell_arg_vectors=0;
  int shell_result;


  int shell_seen[1];
  int shell_index=-1;
  struct option shell_options[]= { { "values",required_argument,0,0 },
                                   { "help",no_argument,0,1 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 1*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* values */
      values=&v_values; igraph_vector_init(values, 0); 
  shell_arg_values=strdup(optarg);
      break;
    case 1:
      shell_igraph_arpack_rssolve_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<1; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_arpack_rssolve(0, 0, 0, 0, values, 0);

  /* Write the result */
  if (values) { shell_write_vector(values, shell_arg_values); 
  igraph_vector_destroy(values); }

  return 0;
}

/*-------------------------------------------/
/ igraph_arpack_rnsolve                      /
/-------------------------------------------*/
void shell_igraph_arpack_rnsolve_usage(char **argv) {
  printf("%s --values=<values>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_arpack_rnsolve(int argc, char **argv) {





  igraph_vector_t v_values; igraph_vector_t *values=0;

  char* shell_arg_options=0;
  char* shell_arg_values=0;
  char* shell_arg_vectors=0;
  int shell_result;


  int shell_seen[1];
  int shell_index=-1;
  struct option shell_options[]= { { "values",required_argument,0,0 },
                                   { "help",no_argument,0,1 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 1*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* values */
      values=&v_values; igraph_vector_init(values, 0); 
  shell_arg_values=strdup(optarg);
      break;
    case 1:
      shell_igraph_arpack_rnsolve_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<1; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_arpack_rnsolve(0, 0, 0, 0, values, 0);

  /* Write the result */
  if (values) { shell_write_vector(values, shell_arg_values); 
  igraph_vector_destroy(values); }

  return 0;
}

/*-------------------------------------------/
/ igraph_arpack_unpack_complex               /
/-------------------------------------------*/
void shell_igraph_arpack_unpack_complex_usage(char **argv) {
  printf("%s --vectors=<vectors> --vectors-out=<vectors-out> --values=<values> --values-out=<values-out> --nev=<nev>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_arpack_unpack_complex(int argc, char **argv) {

  igraph_matrix_t vectors;
  igraph_matrix_t values;
  igraph_real_t nev;
  char* shell_arg_vectors=0;
  char* shell_arg_values=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "vectors",required_argument,0,0 },
                                   { "vectors-out",required_argument,0,1 },
                                   { "values",required_argument,0,2 },
                                   { "values-out",required_argument,0,3 },
                                   { "nev",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* vectors */
      shell_read_matrix(&vectors, optarg);
      break;
    case 1: /* vectors-out */
      shell_arg_vectors=strdup(optarg); 
  igraph_matrix_init(&vectors, 0, 0);
      break;
    case 2: /* values */
      shell_read_matrix(&values, optarg);
      break;
    case 3: /* values-out */
      shell_arg_values=strdup(optarg); 
  igraph_matrix_init(&values, 0, 0);
      break;
    case 4: /* nev */
      shell_read_real(&nev, optarg);
      break;
    case 5:
      shell_igraph_arpack_unpack_complex_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_arpack_unpack_complex(&vectors, &values, nev);

  /* Write the result */
  igraph_matrix_destroy(&vectors);
  shell_write_matrix(&vectors, shell_arg_vectors); 
  igraph_matrix_destroy(&vectors);
  igraph_matrix_destroy(&values);
  shell_write_matrix(&values, shell_arg_values); 
  igraph_matrix_destroy(&values);

  return 0;
}

/*-------------------------------------------/
/ igraph_unfold_tree                         /
/-------------------------------------------*/
void shell_igraph_unfold_tree_usage(char **argv) {
  printf("%s --graph=<graph> --tree=<tree> --mode=<mode> --roots=<roots> --vertex_index=<vertex_index>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_unfold_tree(int argc, char **argv) {

  igraph_t graph;
  igraph_t tree;
  igraph_neimode_t mode=IGRAPH_ALL;
  igraph_vector_t roots;
  igraph_vector_t v_vertex_index; igraph_vector_t *vertex_index=0;
  char* shell_arg_tree=0;
  char* shell_arg_vertex_index=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "tree",required_argument,0,1 },
                                   { "mode",required_argument,0,2 },
                                   { "roots",required_argument,0,3 },
                                   { "vertex_index",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* tree */
      shell_arg_tree=strdup(optarg);
      break;
    case 2: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 3: /* roots */
      shell_read_vector(&roots, optarg);
      break;
    case 4: /* vertex_index */
      vertex_index=&v_vertex_index; igraph_vector_init(vertex_index, 0); 
  shell_arg_vertex_index=strdup(optarg);
      break;
    case 5:
      shell_igraph_unfold_tree_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_unfold_tree(&graph, &tree, mode, &roots, vertex_index);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&tree, shell_arg_tree); 
  igraph_destroy(&tree);
  igraph_vector_destroy(&roots);
  if (vertex_index) { shell_write_vector(vertex_index, shell_arg_vertex_index); 
  igraph_vector_destroy(vertex_index); }

  return 0;
}

/*-------------------------------------------/
/ igraph_laplacian                           /
/-------------------------------------------*/
void shell_igraph_laplacian_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --normalized=<normalized>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_laplacian(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_bool_t normalized=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "normalized",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* normalized */
      shell_read_boolean(&normalized, optarg);
      break;
    case 3:
      shell_igraph_laplacian_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_laplacian(&graph, &res, normalized);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_is_mutual                           /
/-------------------------------------------*/
void shell_igraph_is_mutual_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --es=<es>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_is_mutual(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_bool_t res;
  igraph_vector_t v_es; igraph_es_t es=igraph_ess_all(IGRAPH_EDGEORDER_ID);
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "es",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_bool_init(&res, 0);
      break;
    case 2: /* es */
      shell_read_vector(&v_es, optarg); igraph_es_vector(&es, &v_es);
      break;
    case 3:
      shell_igraph_is_mutual_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_is_mutual(&graph, &res, es);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector_bool(&res, shell_arg_res); 
  igraph_vector_bool_destroy(&res);
  if (!igraph_es_is_all(&es)) { igraph_es_destroy(&es); }

  return 0;
}

/*-------------------------------------------/
/ igraph_avg_nearest_neighbor_degree         /
/-------------------------------------------*/
void shell_igraph_avg_nearest_neighbor_degree_usage(char **argv) {
  printf("%s --graph=<graph> --vids=<vids> --knn=<knn> --knnk=<knnk>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_avg_nearest_neighbor_degree(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_vector_t v_knn; igraph_vector_t *knn=0;
  igraph_vector_t v_knnk; igraph_vector_t *knnk=0;

  char* shell_arg_knn=0;
  char* shell_arg_knnk=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "vids",required_argument,0,1 },
                                   { "knn",required_argument,0,2 },
                                   { "knnk",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[1]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 2: /* knn */
      knn=&v_knn; igraph_vector_init(knn, 0); 
  shell_arg_knn=strdup(optarg);
      break;
    case 3: /* knnk */
      knnk=&v_knnk; igraph_vector_init(knnk, 0); 
  shell_arg_knnk=strdup(optarg);
      break;
    case 4:
      shell_igraph_avg_nearest_neighbor_degree_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_avg_nearest_neighbor_degree(&graph, vids, knn, knnk, 0);

  /* Write the result */
  igraph_destroy(&graph);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }
  if (knn) { shell_write_vector(knn, shell_arg_knn); 
  igraph_vector_destroy(knn); }
  if (knnk) { shell_write_vector(knnk, shell_arg_knnk); 
  igraph_vector_destroy(knnk); }

  return 0;
}

/*-------------------------------------------/
/ igraph_strength                            /
/-------------------------------------------*/
void shell_igraph_strength_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --mode=<mode> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_strength(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_neimode_t mode=IGRAPH_ALL;
  igraph_bool_t loops=1;

  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "loops",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 5:
      shell_igraph_strength_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_strength(&graph, &res, vids, mode, loops, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_bipartite_projection_size           /
/-------------------------------------------*/
void shell_igraph_bipartite_projection_size_usage(char **argv) {
  printf("%s --graph=<graph> --vcount1=<vcount1> --ecount1=<ecount1> --vcount2=<vcount2> --ecount2=<ecount2>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_bipartite_projection_size(int argc, char **argv) {

  igraph_t graph;

  igraph_integer_t vcount1;
  igraph_integer_t ecount1;
  igraph_integer_t vcount2;
  igraph_integer_t ecount2;
  char* shell_arg_vcount1=0;
  char* shell_arg_ecount1=0;
  char* shell_arg_vcount2=0;
  char* shell_arg_ecount2=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "vcount1",required_argument,0,1 },
                                   { "ecount1",required_argument,0,2 },
                                   { "vcount2",required_argument,0,3 },
                                   { "ecount2",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* vcount1 */
      shell_arg_vcount1=strdup(optarg);
      break;
    case 2: /* ecount1 */
      shell_arg_ecount1=strdup(optarg);
      break;
    case 3: /* vcount2 */
      shell_arg_vcount2=strdup(optarg);
      break;
    case 4: /* ecount2 */
      shell_arg_ecount2=strdup(optarg);
      break;
    case 5:
      shell_igraph_bipartite_projection_size_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_bipartite_projection_size(&graph, types, &vcount1, &ecount1, &vcount2, &ecount2);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(vcount1, shell_arg_vcount1);
  shell_write_integer(ecount1, shell_arg_ecount1);
  shell_write_integer(vcount2, shell_arg_vcount2);
  shell_write_integer(ecount2, shell_arg_ecount2);

  return 0;
}

/*-------------------------------------------/
/ igraph_bipartite_projection                /
/-------------------------------------------*/
void shell_igraph_bipartite_projection_usage(char **argv) {
  printf("%s --graph=<graph> --proj1=<proj1> --proj2=<proj2> --probe1=<probe1>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_bipartite_projection(int argc, char **argv) {

  igraph_t graph;

  igraph_t proj1;
  igraph_t proj2;
  igraph_integer_t probe1=-1;
  char* shell_arg_proj1=0;
  char* shell_arg_proj2=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "proj1",required_argument,0,1 },
                                   { "proj2",required_argument,0,2 },
                                   { "probe1",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* proj1 */
      shell_arg_proj1=strdup(optarg);
      break;
    case 2: /* proj2 */
      shell_arg_proj2=strdup(optarg);
      break;
    case 3: /* probe1 */
      shell_read_integer(&probe1, optarg);
      break;
    case 4:
      shell_igraph_bipartite_projection_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_bipartite_projection(&graph, types, &proj1, &proj2, probe1);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&proj1, shell_arg_proj1); 
  igraph_destroy(&proj1);
  shell_write_graph(&proj2, shell_arg_proj2); 
  igraph_destroy(&proj2);

  return 0;
}

/*-------------------------------------------/
/ igraph_create_bipartite                    /
/-------------------------------------------*/
void shell_igraph_create_bipartite_usage(char **argv) {
  printf("%s --graph=<graph> --types=<types> --edges=<edges> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_create_bipartite(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_bool_t types;
  igraph_vector_t edges;
  igraph_bool_t directed=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "types",required_argument,0,1 },
                                   { "edges",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* types */
      
      break;
    case 2: /* edges */
      shell_read_vector(&edges, optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4:
      shell_igraph_create_bipartite_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_create_bipartite(&graph, &types, &edges, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  igraph_vector_destroy(&edges);

  return 0;
}

/*-------------------------------------------/
/ igraph_incidence                           /
/-------------------------------------------*/
void shell_igraph_incidence_usage(char **argv) {
  printf("%s --graph=<graph> --types=<types> --incidence=<incidence> --directed=<directed> --mode=<mode> --multiple=<multiple>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_incidence(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_bool_t types;
  igraph_matrix_t incidence;
  igraph_bool_t directed=0;
  igraph_neimode_t mode=IGRAPH_ALL;
  igraph_bool_t multiple=0;
  char* shell_arg_graph=0;
  char* shell_arg_types=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "types",required_argument,0,1 },
                                   { "incidence",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "mode",required_argument,0,4 },
                                   { "multiple",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* types */
      igraph_vector_bool_init(&types, 0);
      break;
    case 2: /* incidence */
      shell_read_matrix(&incidence, optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 5: /* multiple */
      shell_read_boolean(&multiple, optarg);
      break;
    case 6:
      shell_igraph_incidence_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_incidence(&graph, &types, &incidence, directed, mode, multiple);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  shell_write_vector_bool(&types, shell_arg_types); 
  igraph_vector_bool_destroy(&types);
  igraph_matrix_destroy(&incidence);

  return 0;
}

/*-------------------------------------------/
/ igraph_get_incidence                       /
/-------------------------------------------*/
void shell_igraph_get_incidence_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --row_ids=<row_ids> --col_ids=<col_ids>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_get_incidence(int argc, char **argv) {

  igraph_t graph;

  igraph_matrix_t res;
  igraph_vector_t v_row_ids; igraph_vector_t *row_ids=0;
  igraph_vector_t v_col_ids; igraph_vector_t *col_ids=0;
  char* shell_arg_res=0;
  char* shell_arg_row_ids=0;
  char* shell_arg_col_ids=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "row_ids",required_argument,0,2 },
                                   { "col_ids",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* row_ids */
      row_ids=&v_row_ids; igraph_vector_init(row_ids, 0); 
  shell_arg_row_ids=strdup(optarg);
      break;
    case 3: /* col_ids */
      col_ids=&v_col_ids; igraph_vector_init(col_ids, 0); 
  shell_arg_col_ids=strdup(optarg);
      break;
    case 4:
      shell_igraph_get_incidence_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_get_incidence(&graph, types, &res, row_ids, col_ids);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (row_ids) { shell_write_vector(row_ids, shell_arg_row_ids); 
  igraph_vector_destroy(row_ids); }
  if (col_ids) { shell_write_vector(col_ids, shell_arg_col_ids); 
  igraph_vector_destroy(col_ids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_is_bipartite                        /
/-------------------------------------------*/
void shell_igraph_is_bipartite_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_is_bipartite(int argc, char **argv) {

  igraph_t graph;
  igraph_bool_t res;

  char* shell_arg_res=0;
  char* shell_arg_type=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      
      break;
    case 2:
      shell_igraph_is_bipartite_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_is_bipartite(&graph, &res, type);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_boolean(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_clusters                            /
/-------------------------------------------*/
void shell_igraph_clusters_usage(char **argv) {
  printf("%s --graph=<graph> --membership=<membership> --csize=<csize> --no=<no> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_clusters(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t membership;
  igraph_vector_t csize;
  igraph_integer_t no;
  igraph_connectedness_t mode=IGRAPH_WEAK;
  char* shell_arg_membership=0;
  char* shell_arg_csize=0;
  char* shell_arg_no=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "membership",required_argument,0,1 },
                                   { "csize",required_argument,0,2 },
                                   { "no",required_argument,0,3 },
                                   { "mode",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* membership */
      shell_arg_membership=strdup(optarg); 
  igraph_vector_init(&membership, 0);
      break;
    case 2: /* csize */
      shell_arg_csize=strdup(optarg); 
  igraph_vector_init(&csize, 0);
      break;
    case 3: /* no */
      shell_arg_no=strdup(optarg);
      break;
    case 4: /* mode */
      shell_read_enum(&mode, optarg, "weak", 1, "strong", 2, 0);
      break;
    case 5:
      shell_igraph_clusters_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_clusters(&graph, &membership, &csize, &no, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&membership, shell_arg_membership); 
  igraph_vector_destroy(&membership);
  shell_write_vector(&csize, shell_arg_csize); 
  igraph_vector_destroy(&csize);
  shell_write_integer(no, shell_arg_no);

  return 0;
}

/*-------------------------------------------/
/ igraph_is_connected                        /
/-------------------------------------------*/
void shell_igraph_is_connected_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_is_connected(int argc, char **argv) {

  igraph_t graph;
  igraph_bool_t res;
  igraph_connectedness_t mode=IGRAPH_WEAK;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "mode",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      
      break;
    case 2: /* mode */
      shell_read_enum(&mode, optarg, "weak", 1, "strong", 2, 0);
      break;
    case 3:
      shell_igraph_is_connected_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_is_connected(&graph, &res, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_boolean(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_decompose                           /
/-------------------------------------------*/
void shell_igraph_decompose_usage(char **argv) {
  printf("%s --graph=<graph> --components=<components> --mode=<mode> --maxcompno=<maxcompno> --minelements=<minelements>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_decompose(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_ptr_t components;
  igraph_connectedness_t mode=IGRAPH_WEAK;
  long int maxcompno=-1;
  long int minelements=1;
  char* shell_arg_components=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "components",required_argument,0,1 },
                                   { "mode",required_argument,0,2 },
                                   { "maxcompno",required_argument,0,3 },
                                   { "minelements",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* components */
      igraph_vector_ptr_init(&components, 0);
      break;
    case 2: /* mode */
      shell_read_enum(&mode, optarg, "weak", 1, "strong", 2, 0);
      break;
    case 3: /* maxcompno */
      shell_read_longint(&maxcompno, optarg);
      break;
    case 4: /* minelements */
      shell_read_longint(&minelements, optarg);
      break;
    case 5:
      shell_igraph_decompose_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_decompose(&graph, &components, mode, maxcompno, minelements);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graphlist(&components, shell_arg_components); 
  shell_free_graphlist(&components); 
  igraph_vector_ptr_destroy(&components);

  return 0;
}

/*-------------------------------------------/
/ igraph_articulation_points                 /
/-------------------------------------------*/
void shell_igraph_articulation_points_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_articulation_points(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2:
      shell_igraph_articulation_points_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_articulation_points(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_biconnected_components              /
/-------------------------------------------*/
void shell_igraph_biconnected_components_usage(char **argv) {
  printf("%s --graph=<graph> --no=<no> --components=<components> --articulation_points=<articulation_points>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_biconnected_components(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t no;
  igraph_vector_ptr_t components;
  igraph_vector_t articulation_points;
  char* shell_arg_no=0;
  char* shell_arg_components=0;
  char* shell_arg_articulation_points=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "no",required_argument,0,1 },
                                   { "components",required_argument,0,2 },
                                   { "articulation_points",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* no */
      shell_arg_no=strdup(optarg);
      break;
    case 2: /* components */
      igraph_vector_ptr_init(&components, 0);
      break;
    case 3: /* articulation_points */
      shell_arg_articulation_points=strdup(optarg); 
  igraph_vector_init(&articulation_points, 0);
      break;
    case 4:
      shell_igraph_biconnected_components_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_biconnected_components(&graph, &no, &components, &articulation_points);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(no, shell_arg_no);
  shell_write_vectorlist(&components, shell_arg_components); 
  shell_free_vectorlist(&components); 
  igraph_vector_ptr_destroy(&components);
  shell_write_vector(&articulation_points, shell_arg_articulation_points); 
  igraph_vector_destroy(&articulation_points);

  return 0;
}

/*-------------------------------------------/
/ igraph_cliques                             /
/-------------------------------------------*/
void shell_igraph_cliques_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --min_size=<min_size> --max_size=<max_size>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_cliques(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_ptr_t res;
  igraph_integer_t min_size=0;
  igraph_integer_t max_size=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "min_size",required_argument,0,2 },
                                   { "max_size",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_ptr_init(&res, 0);
      break;
    case 2: /* min_size */
      shell_read_integer(&min_size, optarg);
      break;
    case 3: /* max_size */
      shell_read_integer(&max_size, optarg);
      break;
    case 4:
      shell_igraph_cliques_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_cliques(&graph, &res, min_size, max_size);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vectorlist(&res, shell_arg_res); 
  shell_free_vectorlist(&res); 
  igraph_vector_ptr_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_largest_cliques                     /
/-------------------------------------------*/
void shell_igraph_largest_cliques_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_largest_cliques(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_ptr_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_ptr_init(&res, 0);
      break;
    case 2:
      shell_igraph_largest_cliques_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_largest_cliques(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vectorlist(&res, shell_arg_res); 
  shell_free_vectorlist(&res); 
  igraph_vector_ptr_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_maximal_cliques                     /
/-------------------------------------------*/
void shell_igraph_maximal_cliques_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_maximal_cliques(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_ptr_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_ptr_init(&res, 0);
      break;
    case 2:
      shell_igraph_maximal_cliques_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_maximal_cliques(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vectorlist(&res, shell_arg_res); 
  shell_free_vectorlist(&res); 
  igraph_vector_ptr_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_clique_number                       /
/-------------------------------------------*/
void shell_igraph_clique_number_usage(char **argv) {
  printf("%s --graph=<graph> --no=<no>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_clique_number(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t no;
  char* shell_arg_no=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "no",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* no */
      shell_arg_no=strdup(optarg);
      break;
    case 2:
      shell_igraph_clique_number_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_clique_number(&graph, &no);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(no, shell_arg_no);

  return 0;
}

/*-------------------------------------------/
/ igraph_independent_vertex_sets             /
/-------------------------------------------*/
void shell_igraph_independent_vertex_sets_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --min_size=<min_size> --max_size=<max_size>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_independent_vertex_sets(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_ptr_t res;
  igraph_integer_t min_size=0;
  igraph_integer_t max_size=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "min_size",required_argument,0,2 },
                                   { "max_size",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_ptr_init(&res, 0);
      break;
    case 2: /* min_size */
      shell_read_integer(&min_size, optarg);
      break;
    case 3: /* max_size */
      shell_read_integer(&max_size, optarg);
      break;
    case 4:
      shell_igraph_independent_vertex_sets_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_independent_vertex_sets(&graph, &res, min_size, max_size);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vectorlist(&res, shell_arg_res); 
  shell_free_vectorlist(&res); 
  igraph_vector_ptr_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_largest_independent_vertex_sets     /
/-------------------------------------------*/
void shell_igraph_largest_independent_vertex_sets_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_largest_independent_vertex_sets(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_ptr_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_ptr_init(&res, 0);
      break;
    case 2:
      shell_igraph_largest_independent_vertex_sets_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_largest_independent_vertex_sets(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vectorlist(&res, shell_arg_res); 
  shell_free_vectorlist(&res); 
  igraph_vector_ptr_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_maximal_independent_vertex_sets     /
/-------------------------------------------*/
void shell_igraph_maximal_independent_vertex_sets_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_maximal_independent_vertex_sets(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_ptr_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      igraph_vector_ptr_init(&res, 0);
      break;
    case 2:
      shell_igraph_maximal_independent_vertex_sets_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_maximal_independent_vertex_sets(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vectorlist(&res, shell_arg_res); 
  shell_free_vectorlist(&res); 
  igraph_vector_ptr_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_independence_number                 /
/-------------------------------------------*/
void shell_igraph_independence_number_usage(char **argv) {
  printf("%s --graph=<graph> --no=<no>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_independence_number(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t no;
  char* shell_arg_no=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "no",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* no */
      shell_arg_no=strdup(optarg);
      break;
    case 2:
      shell_igraph_independence_number_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_independence_number(&graph, &no);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(no, shell_arg_no);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_random                       /
/-------------------------------------------*/
void shell_igraph_layout_random_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_random(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2:
      shell_igraph_layout_random_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_random(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_circle                       /
/-------------------------------------------*/
void shell_igraph_layout_circle_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_circle(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2:
      shell_igraph_layout_circle_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_circle(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_fruchterman_reingold         /
/-------------------------------------------*/
void shell_igraph_layout_fruchterman_reingold_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --niter=<niter> --maxdelta=<maxdelta> --area=<area> --coolexp=<coolexp> --repulserad=<repulserad> --weight=<weight>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_fruchterman_reingold(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_integer_t niter=500;
  igraph_real_t maxdelta=100;
  igraph_real_t area=1000;
  igraph_real_t coolexp=1.5;
  igraph_real_t repulserad=10000;

  igraph_vector_t v_weight; igraph_vector_t *weight=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "niter",required_argument,0,2 },
                                   { "maxdelta",required_argument,0,3 },
                                   { "area",required_argument,0,4 },
                                   { "coolexp",required_argument,0,5 },
                                   { "repulserad",required_argument,0,6 },
                                   { "weight",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 3: /* maxdelta */
      shell_read_real(&maxdelta, optarg);
      break;
    case 4: /* area */
      shell_read_real(&area, optarg);
      break;
    case 5: /* coolexp */
      shell_read_real(&coolexp, optarg);
      break;
    case 6: /* repulserad */
      shell_read_real(&repulserad, optarg);
      break;
    case 7: /* weight */
      weight=&v_weight; shell_read_vector(weight, optarg);
      break;
    case 8:
      shell_igraph_layout_fruchterman_reingold_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_fruchterman_reingold(&graph, &res, niter, maxdelta, area, coolexp, repulserad, 0, weight);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (weight) { igraph_vector_destroy(weight); }

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_grid_fruchterman_reingold    /
/-------------------------------------------*/
void shell_igraph_layout_grid_fruchterman_reingold_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --niter=<niter> --maxdelta=<maxdelta> --area=<area> --coolexp=<coolexp> --repulserad=<repulserad> --cellsize=<cellsize>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_grid_fruchterman_reingold(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_integer_t niter=500;
  igraph_real_t maxdelta=100;
  igraph_real_t area=1000;
  igraph_real_t coolexp=1.5;
  igraph_real_t repulserad=10000;
  igraph_real_t cellsize=100;

  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "niter",required_argument,0,2 },
                                   { "maxdelta",required_argument,0,3 },
                                   { "area",required_argument,0,4 },
                                   { "coolexp",required_argument,0,5 },
                                   { "repulserad",required_argument,0,6 },
                                   { "cellsize",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  shell_seen[7]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 3: /* maxdelta */
      shell_read_real(&maxdelta, optarg);
      break;
    case 4: /* area */
      shell_read_real(&area, optarg);
      break;
    case 5: /* coolexp */
      shell_read_real(&coolexp, optarg);
      break;
    case 6: /* repulserad */
      shell_read_real(&repulserad, optarg);
      break;
    case 7: /* cellsize */
      shell_read_real(&cellsize, optarg);
      break;
    case 8:
      shell_igraph_layout_grid_fruchterman_reingold_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_grid_fruchterman_reingold(&graph, &res, niter, maxdelta, area, coolexp, repulserad, cellsize, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_kamada_kawai                 /
/-------------------------------------------*/
void shell_igraph_layout_kamada_kawai_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --niter=<niter> --sigma=<sigma> --initemp=<initemp> --coolexp=<coolexp> --kkconst=<kkconst>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_kamada_kawai(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_integer_t niter=1000;
  igraph_real_t sigma=10;
  igraph_real_t initemp=10;
  igraph_real_t coolexp=0.99;
  igraph_real_t kkconst=1000;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[7];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "niter",required_argument,0,2 },
                                   { "sigma",required_argument,0,3 },
                                   { "initemp",required_argument,0,4 },
                                   { "coolexp",required_argument,0,5 },
                                   { "kkconst",required_argument,0,6 },
                                   { "help",no_argument,0,7 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 7*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 3: /* sigma */
      shell_read_real(&sigma, optarg);
      break;
    case 4: /* initemp */
      shell_read_real(&initemp, optarg);
      break;
    case 5: /* coolexp */
      shell_read_real(&coolexp, optarg);
      break;
    case 6: /* kkconst */
      shell_read_real(&kkconst, optarg);
      break;
    case 7:
      shell_igraph_layout_kamada_kawai_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<7; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_kamada_kawai(&graph, &res, niter, sigma, initemp, coolexp, kkconst);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_lgl                          /
/-------------------------------------------*/
void shell_igraph_layout_lgl_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --maxiter=<maxiter> --maxdelta=<maxdelta> --area=<area> --coolexp=<coolexp> --repulserad=<repulserad> --cellsize=<cellsize> --root=<root>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_lgl(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_integer_t maxiter=150;
  igraph_real_t maxdelta=100;
  igraph_real_t area=1000;
  igraph_real_t coolexp=1.5;
  igraph_real_t repulserad=10000;
  igraph_real_t cellsize=100;
  igraph_integer_t root=-1;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[9];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "maxiter",required_argument,0,2 },
                                   { "maxdelta",required_argument,0,3 },
                                   { "area",required_argument,0,4 },
                                   { "coolexp",required_argument,0,5 },
                                   { "repulserad",required_argument,0,6 },
                                   { "cellsize",required_argument,0,7 },
                                   { "root",required_argument,0,8 },
                                   { "help",no_argument,0,9 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 9*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  shell_seen[7]=2;
  shell_seen[8]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* maxiter */
      shell_read_integer(&maxiter, optarg);
      break;
    case 3: /* maxdelta */
      shell_read_real(&maxdelta, optarg);
      break;
    case 4: /* area */
      shell_read_real(&area, optarg);
      break;
    case 5: /* coolexp */
      shell_read_real(&coolexp, optarg);
      break;
    case 6: /* repulserad */
      shell_read_real(&repulserad, optarg);
      break;
    case 7: /* cellsize */
      shell_read_real(&cellsize, optarg);
      break;
    case 8: /* root */
      shell_read_integer(&root, optarg);
      break;
    case 9:
      shell_igraph_layout_lgl_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<9; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_lgl(&graph, &res, maxiter, maxdelta, area, coolexp, repulserad, cellsize, root);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_reingold_tilford             /
/-------------------------------------------*/
void shell_igraph_layout_reingold_tilford_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --root=<root>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_reingold_tilford(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  long int root=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "root",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* root */
      shell_read_longint(&root, optarg);
      break;
    case 3:
      shell_igraph_layout_reingold_tilford_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_reingold_tilford(&graph, &res, root);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_reingold_tilford_circular    /
/-------------------------------------------*/
void shell_igraph_layout_reingold_tilford_circular_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --root=<root>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_reingold_tilford_circular(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  long int root=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "root",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* root */
      shell_read_longint(&root, optarg);
      break;
    case 3:
      shell_igraph_layout_reingold_tilford_circular_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_reingold_tilford_circular(&graph, &res, root);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_random_3d                    /
/-------------------------------------------*/
void shell_igraph_layout_random_3d_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_random_3d(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2:
      shell_igraph_layout_random_3d_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_random_3d(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_sphere                       /
/-------------------------------------------*/
void shell_igraph_layout_sphere_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_sphere(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2:
      shell_igraph_layout_sphere_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_sphere(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_fruchterman_reingold_3d      /
/-------------------------------------------*/
void shell_igraph_layout_fruchterman_reingold_3d_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --niter=<niter> --maxdelta=<maxdelta> --volume=<volume> --coolexp=<coolexp> --repulserad=<repulserad> --weight=<weight>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_fruchterman_reingold_3d(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_integer_t niter=500;
  igraph_real_t maxdelta=100;
  igraph_real_t volume=1000;
  igraph_real_t coolexp=1.5;
  igraph_real_t repulserad=10000;

  igraph_vector_t v_weight; igraph_vector_t *weight=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "niter",required_argument,0,2 },
                                   { "maxdelta",required_argument,0,3 },
                                   { "volume",required_argument,0,4 },
                                   { "coolexp",required_argument,0,5 },
                                   { "repulserad",required_argument,0,6 },
                                   { "weight",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 3: /* maxdelta */
      shell_read_real(&maxdelta, optarg);
      break;
    case 4: /* volume */
      shell_read_real(&volume, optarg);
      break;
    case 5: /* coolexp */
      shell_read_real(&coolexp, optarg);
      break;
    case 6: /* repulserad */
      shell_read_real(&repulserad, optarg);
      break;
    case 7: /* weight */
      weight=&v_weight; shell_read_vector(weight, optarg);
      break;
    case 8:
      shell_igraph_layout_fruchterman_reingold_3d_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_fruchterman_reingold_3d(&graph, &res, niter, maxdelta, volume, coolexp, repulserad, 0, weight);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (weight) { igraph_vector_destroy(weight); }

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_kamada_kawai_3d              /
/-------------------------------------------*/
void shell_igraph_layout_kamada_kawai_3d_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --niter=<niter> --sigma=<sigma> --initemp=<initemp> --coolexp=<coolexp> --kkconst=<kkconst>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_kamada_kawai_3d(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_integer_t niter=1000;
  igraph_real_t sigma=10;
  igraph_real_t initemp=10;
  igraph_real_t coolexp=0.99;
  igraph_real_t kkconst=1000;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[7];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "niter",required_argument,0,2 },
                                   { "sigma",required_argument,0,3 },
                                   { "initemp",required_argument,0,4 },
                                   { "coolexp",required_argument,0,5 },
                                   { "kkconst",required_argument,0,6 },
                                   { "help",no_argument,0,7 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 7*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 3: /* sigma */
      shell_read_real(&sigma, optarg);
      break;
    case 4: /* initemp */
      shell_read_real(&initemp, optarg);
      break;
    case 5: /* coolexp */
      shell_read_real(&coolexp, optarg);
      break;
    case 6: /* kkconst */
      shell_read_real(&kkconst, optarg);
      break;
    case 7:
      shell_igraph_layout_kamada_kawai_3d_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<7; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_kamada_kawai_3d(&graph, &res, niter, sigma, initemp, coolexp, kkconst);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_graphopt                     /
/-------------------------------------------*/
void shell_igraph_layout_graphopt_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --niter=<niter> --node_charge=<node_charge> --node_mass=<node_mass> --spring_length=<spring_length> --spring_constant=<spring_constant> --max_sa_movement=<max_sa_movement>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_graphopt(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_integer_t niter=500;
  igraph_real_t node_charge=0.001;
  igraph_real_t node_mass=30;
  igraph_integer_t spring_length=0;
  igraph_real_t spring_constant=1;
  igraph_real_t max_sa_movement=5;

  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "niter",required_argument,0,2 },
                                   { "node_charge",required_argument,0,3 },
                                   { "node_mass",required_argument,0,4 },
                                   { "spring_length",required_argument,0,5 },
                                   { "spring_constant",required_argument,0,6 },
                                   { "max_sa_movement",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  shell_seen[7]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 3: /* node_charge */
      shell_read_real(&node_charge, optarg);
      break;
    case 4: /* node_mass */
      shell_read_real(&node_mass, optarg);
      break;
    case 5: /* spring_length */
      shell_read_integer(&spring_length, optarg);
      break;
    case 6: /* spring_constant */
      shell_read_real(&spring_constant, optarg);
      break;
    case 7: /* max_sa_movement */
      shell_read_real(&max_sa_movement, optarg);
      break;
    case 8:
      shell_igraph_layout_graphopt_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_graphopt(&graph, &res, niter, node_charge, node_mass, spring_length, spring_constant, max_sa_movement, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_drl                          /
/-------------------------------------------*/
void shell_igraph_layout_drl_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --res-out=<res-out> --use_seed=<use_seed> --weights=<weights>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_drl(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_bool_t use_seed=0;

  igraph_vector_t v_weights; igraph_vector_t *weights=0=NULL;

  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "res-out",required_argument,0,2 },
                                   { "use_seed",required_argument,0,3 },
                                   { "weights",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_read_matrix(&res, optarg);
      break;
    case 2: /* res-out */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 3: /* use_seed */
      shell_read_boolean(&use_seed, optarg);
      break;
    case 4: /* weights */
      weights=&v_weights; shell_read_vector(weights, optarg);
      break;
    case 5:
      shell_igraph_layout_drl_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_drl(&graph, &res, use_seed, options, weights, fixed);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_matrix_destroy(&res);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (weights) { igraph_vector_destroy(weights); }

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_drl_3d                       /
/-------------------------------------------*/
void shell_igraph_layout_drl_3d_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --res-out=<res-out> --use_seed=<use_seed> --weights=<weights>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_drl_3d(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_bool_t use_seed=0;

  igraph_vector_t v_weights; igraph_vector_t *weights=0=NULL;

  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "res-out",required_argument,0,2 },
                                   { "use_seed",required_argument,0,3 },
                                   { "weights",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_read_matrix(&res, optarg);
      break;
    case 2: /* res-out */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 3: /* use_seed */
      shell_read_boolean(&use_seed, optarg);
      break;
    case 4: /* weights */
      weights=&v_weights; shell_read_vector(weights, optarg);
      break;
    case 5:
      shell_igraph_layout_drl_3d_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_drl_3d(&graph, &res, use_seed, options, weights, fixed);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_matrix_destroy(&res);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (weights) { igraph_vector_destroy(weights); }

  return 0;
}

/*-------------------------------------------/
/ igraph_layout_merge_dla                    /
/-------------------------------------------*/
void shell_igraph_layout_merge_dla_usage(char **argv) {
  printf("%s --graphs=<graphs> --coords=<coords> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_layout_merge_dla(int argc, char **argv) {

  igraph_vector_ptr_t graphs;
  igraph_vector_ptr_t coords;
  igraph_matrix_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graphs",required_argument,0,0 },
                                   { "coords",required_argument,0,1 },
                                   { "res",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graphs */
      shell_read_graphlist(&graphs, optarg);
      break;
    case 1: /* coords */
      shell_read_matrixlist(&coords, optarg);
      break;
    case 2: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 3:
      shell_igraph_layout_merge_dla_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_layout_merge_dla(&graphs, &coords, &res);

  /* Write the result */
  shell_free_graphlist(&graphs); 
  igraph_vector_ptr_destroy(&graphs);
  shell_free_matrixlist(&coords); 
  igraph_vector_ptr_destroy(&coords);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_cocitation                          /
/-------------------------------------------*/
void shell_igraph_cocitation_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_cocitation(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3:
      shell_igraph_cocitation_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_cocitation(&graph, &res, vids);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_bibcoupling                         /
/-------------------------------------------*/
void shell_igraph_bibcoupling_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_bibcoupling(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3:
      shell_igraph_bibcoupling_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_bibcoupling(&graph, &res, vids);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_similarity_jaccard                  /
/-------------------------------------------*/
void shell_igraph_similarity_jaccard_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --mode=<mode> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_similarity_jaccard(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_neimode_t mode=IGRAPH_ALL;
  igraph_bool_t loops=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "loops",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 5:
      shell_igraph_similarity_jaccard_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_similarity_jaccard(&graph, &res, vids, mode, loops);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_similarity_dice                     /
/-------------------------------------------*/
void shell_igraph_similarity_dice_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --mode=<mode> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_similarity_dice(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_neimode_t mode=IGRAPH_ALL;
  igraph_bool_t loops=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "loops",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 5:
      shell_igraph_similarity_dice_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_similarity_dice(&graph, &res, vids, mode, loops);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_similarity_inverse_log_weighted     /
/-------------------------------------------*/
void shell_igraph_similarity_inverse_log_weighted_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --vids=<vids> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_similarity_inverse_log_weighted(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_vector_t v_vids; igraph_vs_t vids=igraph_vss_all();
  igraph_neimode_t mode=IGRAPH_ALL;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "vids",required_argument,0,2 },
                                   { "mode",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* vids */
      shell_read_vector(&v_vids, optarg); igraph_vs_vector(&vids, &v_vids);
      break;
    case 3: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 4:
      shell_igraph_similarity_inverse_log_weighted_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_similarity_inverse_log_weighted(&graph, &res, vids, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);
  if (!igraph_vs_is_all(&vids)) { igraph_vector_destroy(&v_vids); }

  return 0;
}

/*-------------------------------------------/
/ igraph_community_spinglass                 /
/-------------------------------------------*/
void shell_igraph_community_spinglass_usage(char **argv) {
  printf("%s --graph=<graph> --weights=<weights> --modularity=<modularity> --temperature=<temperature> --membership=<membership> --csize=<csize> --spins=<spins> --parupdate=<parupdate> --starttemp=<starttemp> --stoptemp=<stoptemp> --coolfact=<coolfact> --update_rule=<update_rule> --gamma=<gamma>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_community_spinglass(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t v_weights; igraph_vector_t *weights=0;
  igraph_real_t modularity;
  igraph_real_t temperature;
  igraph_vector_t membership;
  igraph_vector_t csize;
  igraph_integer_t spins=25;
  igraph_bool_t parupdate=0;
  igraph_real_t starttemp=1;
  igraph_real_t stoptemp=0.01;
  igraph_real_t coolfact=0.99;
  igraph_spincomm_update_t update_rule=IGRAPH_SPINCOMM_UPDATE_CONFIG;
  igraph_real_t gamma=1.0;
  char* shell_arg_modularity=0;
  char* shell_arg_temperature=0;
  char* shell_arg_membership=0;
  char* shell_arg_csize=0;
  int shell_result;


  int shell_seen[13];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "weights",required_argument,0,1 },
                                   { "modularity",required_argument,0,2 },
                                   { "temperature",required_argument,0,3 },
                                   { "membership",required_argument,0,4 },
                                   { "csize",required_argument,0,5 },
                                   { "spins",required_argument,0,6 },
                                   { "parupdate",required_argument,0,7 },
                                   { "starttemp",required_argument,0,8 },
                                   { "stoptemp",required_argument,0,9 },
                                   { "coolfact",required_argument,0,10 },
                                   { "update_rule",required_argument,0,11 },
                                   { "gamma",required_argument,0,12 },
                                   { "help",no_argument,0,13 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 13*sizeof(int));
  shell_seen[6]=2;
  shell_seen[7]=2;
  shell_seen[8]=2;
  shell_seen[9]=2;
  shell_seen[10]=2;
  shell_seen[11]=2;
  shell_seen[12]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* weights */
      weights=&v_weights; shell_read_vector(weights, optarg);
      break;
    case 2: /* modularity */
      shell_arg_modularity=strdup(optarg);
      break;
    case 3: /* temperature */
      shell_arg_temperature=strdup(optarg);
      break;
    case 4: /* membership */
      shell_arg_membership=strdup(optarg); 
  igraph_vector_init(&membership, 0);
      break;
    case 5: /* csize */
      shell_arg_csize=strdup(optarg); 
  igraph_vector_init(&csize, 0);
      break;
    case 6: /* spins */
      shell_read_integer(&spins, optarg);
      break;
    case 7: /* parupdate */
      shell_read_boolean(&parupdate, optarg);
      break;
    case 8: /* starttemp */
      shell_read_real(&starttemp, optarg);
      break;
    case 9: /* stoptemp */
      shell_read_real(&stoptemp, optarg);
      break;
    case 10: /* coolfact */
      shell_read_real(&coolfact, optarg);
      break;
    case 11: /* update_rule */
      shell_read_enum(&update_rule, optarg, "simple", 0, "config", 1, 0);
      break;
    case 12: /* gamma */
      shell_read_real(&gamma, optarg);
      break;
    case 13:
      shell_igraph_community_spinglass_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<13; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_community_spinglass(&graph, weights, &modularity, &temperature, &membership, &csize, spins, parupdate, starttemp, stoptemp, coolfact, update_rule, gamma);

  /* Write the result */
  igraph_destroy(&graph);
  if (weights) { igraph_vector_destroy(weights); }
  shell_write_real(modularity, shell_arg_modularity);
  shell_write_real(temperature, shell_arg_temperature);
  shell_write_vector(&membership, shell_arg_membership); 
  igraph_vector_destroy(&membership);
  shell_write_vector(&csize, shell_arg_csize); 
  igraph_vector_destroy(&csize);

  return 0;
}

/*-------------------------------------------/
/ igraph_community_spinglass_single          /
/-------------------------------------------*/
void shell_igraph_community_spinglass_single_usage(char **argv) {
  printf("%s --graph=<graph> --weights=<weights> --vertex=<vertex> --community=<community> --cohesion=<cohesion> --adhesion=<adhesion> --inner_links=<inner_links> --outer_links=<outer_links> --spins=<spins> --update_rule=<update_rule> --gamma=<gamma>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_community_spinglass_single(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t v_weights; igraph_vector_t *weights=0;
  igraph_integer_t vertex;
  igraph_vector_t community;
  igraph_real_t cohesion;
  igraph_real_t adhesion;
  igraph_integer_t inner_links;
  igraph_integer_t outer_links;
  igraph_integer_t spins=25;
  igraph_spincomm_update_t update_rule=IGRAPH_SPINCOMM_UPDATE_CONFIG;
  igraph_real_t gamma=1.0;
  char* shell_arg_community=0;
  char* shell_arg_cohesion=0;
  char* shell_arg_adhesion=0;
  char* shell_arg_inner_links=0;
  char* shell_arg_outer_links=0;
  int shell_result;


  int shell_seen[11];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "weights",required_argument,0,1 },
                                   { "vertex",required_argument,0,2 },
                                   { "community",required_argument,0,3 },
                                   { "cohesion",required_argument,0,4 },
                                   { "adhesion",required_argument,0,5 },
                                   { "inner_links",required_argument,0,6 },
                                   { "outer_links",required_argument,0,7 },
                                   { "spins",required_argument,0,8 },
                                   { "update_rule",required_argument,0,9 },
                                   { "gamma",required_argument,0,10 },
                                   { "help",no_argument,0,11 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 11*sizeof(int));
  shell_seen[8]=2;
  shell_seen[9]=2;
  shell_seen[10]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* weights */
      weights=&v_weights; shell_read_vector(weights, optarg);
      break;
    case 2: /* vertex */
      shell_read_integer(&vertex, optarg);
      break;
    case 3: /* community */
      shell_arg_community=strdup(optarg); 
  igraph_vector_init(&community, 0);
      break;
    case 4: /* cohesion */
      shell_arg_cohesion=strdup(optarg);
      break;
    case 5: /* adhesion */
      shell_arg_adhesion=strdup(optarg);
      break;
    case 6: /* inner_links */
      shell_arg_inner_links=strdup(optarg);
      break;
    case 7: /* outer_links */
      shell_arg_outer_links=strdup(optarg);
      break;
    case 8: /* spins */
      shell_read_integer(&spins, optarg);
      break;
    case 9: /* update_rule */
      shell_read_enum(&update_rule, optarg, "simple", 0, "config", 1, 0);
      break;
    case 10: /* gamma */
      shell_read_real(&gamma, optarg);
      break;
    case 11:
      shell_igraph_community_spinglass_single_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<11; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_community_spinglass_single(&graph, weights, vertex, &community, &cohesion, &adhesion, &inner_links, &outer_links, spins, update_rule, gamma);

  /* Write the result */
  igraph_destroy(&graph);
  if (weights) { igraph_vector_destroy(weights); }
  shell_write_vector(&community, shell_arg_community); 
  igraph_vector_destroy(&community);
  shell_write_real(cohesion, shell_arg_cohesion);
  shell_write_real(adhesion, shell_arg_adhesion);
  shell_write_integer(inner_links, shell_arg_inner_links);
  shell_write_integer(outer_links, shell_arg_outer_links);

  return 0;
}

/*-------------------------------------------/
/ igraph_community_walktrap                  /
/-------------------------------------------*/
void shell_igraph_community_walktrap_usage(char **argv) {
  printf("%s --graph=<graph> --weights=<weights> --steps=<steps> --merges=<merges> --modularity=<modularity>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_community_walktrap(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t weights;
  int steps=4;
  igraph_matrix_t merges;
  igraph_vector_t modularity;
  char* shell_arg_merges=0;
  char* shell_arg_modularity=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "weights",required_argument,0,1 },
                                   { "steps",required_argument,0,2 },
                                   { "merges",required_argument,0,3 },
                                   { "modularity",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* weights */
      shell_read_vector(&weights, optarg);
      break;
    case 2: /* steps */
      shell_read_int(&steps, optarg);
      break;
    case 3: /* merges */
      shell_arg_merges=strdup(optarg); 
  igraph_matrix_init(&merges, 0, 0);
      break;
    case 4: /* modularity */
      shell_arg_modularity=strdup(optarg); 
  igraph_vector_init(&modularity, 0);
      break;
    case 5:
      shell_igraph_community_walktrap_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_community_walktrap(&graph, &weights, steps, &merges, &modularity);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&weights);
  shell_write_matrix(&merges, shell_arg_merges); 
  igraph_matrix_destroy(&merges);
  shell_write_vector(&modularity, shell_arg_modularity); 
  igraph_vector_destroy(&modularity);

  return 0;
}

/*-------------------------------------------/
/ igraph_community_edge_betweenness          /
/-------------------------------------------*/
void shell_igraph_community_edge_betweenness_usage(char **argv) {
  printf("%s --graph=<graph> --result=<result> --edge_betweenness=<edge_betweenness> --merges=<merges> --bridges=<bridges> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_community_edge_betweenness(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t result;
  igraph_vector_t edge_betweenness;
  igraph_matrix_t merges;
  igraph_vector_t bridges;
  igraph_bool_t directed=1;
  char* shell_arg_result=0;
  char* shell_arg_edge_betweenness=0;
  char* shell_arg_merges=0;
  char* shell_arg_bridges=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "result",required_argument,0,1 },
                                   { "edge_betweenness",required_argument,0,2 },
                                   { "merges",required_argument,0,3 },
                                   { "bridges",required_argument,0,4 },
                                   { "directed",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* result */
      shell_arg_result=strdup(optarg); 
  igraph_vector_init(&result, 0);
      break;
    case 2: /* edge_betweenness */
      shell_arg_edge_betweenness=strdup(optarg); 
  igraph_vector_init(&edge_betweenness, 0);
      break;
    case 3: /* merges */
      shell_arg_merges=strdup(optarg); 
  igraph_matrix_init(&merges, 0, 0);
      break;
    case 4: /* bridges */
      shell_arg_bridges=strdup(optarg); 
  igraph_vector_init(&bridges, 0);
      break;
    case 5: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 6:
      shell_igraph_community_edge_betweenness_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_community_edge_betweenness(&graph, &result, &edge_betweenness, &merges, &bridges, directed);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&result, shell_arg_result); 
  igraph_vector_destroy(&result);
  shell_write_vector(&edge_betweenness, shell_arg_edge_betweenness); 
  igraph_vector_destroy(&edge_betweenness);
  shell_write_matrix(&merges, shell_arg_merges); 
  igraph_matrix_destroy(&merges);
  shell_write_vector(&bridges, shell_arg_bridges); 
  igraph_vector_destroy(&bridges);

  return 0;
}

/*-------------------------------------------/
/ igraph_community_eb_get_merges             /
/-------------------------------------------*/
void shell_igraph_community_eb_get_merges_usage(char **argv) {
  printf("%s --graph=<graph> --edges=<edges> --merges=<merges> --bridges=<bridges>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_community_eb_get_merges(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t edges;
  igraph_matrix_t merges;
  igraph_vector_t bridges;
  char* shell_arg_merges=0;
  char* shell_arg_bridges=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "edges",required_argument,0,1 },
                                   { "merges",required_argument,0,2 },
                                   { "bridges",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* edges */
      shell_read_vector(&edges, optarg);
      break;
    case 2: /* merges */
      shell_arg_merges=strdup(optarg); 
  igraph_matrix_init(&merges, 0, 0);
      break;
    case 3: /* bridges */
      shell_arg_bridges=strdup(optarg); 
  igraph_vector_init(&bridges, 0);
      break;
    case 4:
      shell_igraph_community_eb_get_merges_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_community_eb_get_merges(&graph, &edges, &merges, &bridges);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&edges);
  shell_write_matrix(&merges, shell_arg_merges); 
  igraph_matrix_destroy(&merges);
  shell_write_vector(&bridges, shell_arg_bridges); 
  igraph_vector_destroy(&bridges);

  return 0;
}

/*-------------------------------------------/
/ igraph_community_fastgreedy                /
/-------------------------------------------*/
void shell_igraph_community_fastgreedy_usage(char **argv) {
  printf("%s --graph=<graph> --weights=<weights> --merges=<merges> --modularity=<modularity>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_community_fastgreedy(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t v_weights; igraph_vector_t *weights=0;
  igraph_matrix_t merges;
  igraph_vector_t modularity;
  char* shell_arg_merges=0;
  char* shell_arg_modularity=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "weights",required_argument,0,1 },
                                   { "merges",required_argument,0,2 },
                                   { "modularity",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* weights */
      weights=&v_weights; shell_read_vector(weights, optarg);
      break;
    case 2: /* merges */
      shell_arg_merges=strdup(optarg); 
  igraph_matrix_init(&merges, 0, 0);
      break;
    case 3: /* modularity */
      shell_arg_modularity=strdup(optarg); 
  igraph_vector_init(&modularity, 0);
      break;
    case 4:
      shell_igraph_community_fastgreedy_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_community_fastgreedy(&graph, weights, &merges, &modularity);

  /* Write the result */
  igraph_destroy(&graph);
  if (weights) { igraph_vector_destroy(weights); }
  shell_write_matrix(&merges, shell_arg_merges); 
  igraph_matrix_destroy(&merges);
  shell_write_vector(&modularity, shell_arg_modularity); 
  igraph_vector_destroy(&modularity);

  return 0;
}

/*-------------------------------------------/
/ igraph_community_to_membership             /
/-------------------------------------------*/
void shell_igraph_community_to_membership_usage(char **argv) {
  printf("%s --graph=<graph> --merges=<merges> --steps=<steps> --membership=<membership> --csize=<csize>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_community_to_membership(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t merges;
  igraph_integer_t steps;
  igraph_vector_t membership;
  igraph_vector_t csize;
  char* shell_arg_membership=0;
  char* shell_arg_csize=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "merges",required_argument,0,1 },
                                   { "steps",required_argument,0,2 },
                                   { "membership",required_argument,0,3 },
                                   { "csize",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* merges */
      shell_read_matrix(&merges, optarg);
      break;
    case 2: /* steps */
      shell_read_integer(&steps, optarg);
      break;
    case 3: /* membership */
      shell_arg_membership=strdup(optarg); 
  igraph_vector_init(&membership, 0);
      break;
    case 4: /* csize */
      shell_arg_csize=strdup(optarg); 
  igraph_vector_init(&csize, 0);
      break;
    case 5:
      shell_igraph_community_to_membership_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_community_to_membership(&graph, &merges, steps, &membership, &csize);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_matrix_destroy(&merges);
  shell_write_vector(&membership, shell_arg_membership); 
  igraph_vector_destroy(&membership);
  shell_write_vector(&csize, shell_arg_csize); 
  igraph_vector_destroy(&csize);

  return 0;
}

/*-------------------------------------------/
/ igraph_le_community_to_membership          /
/-------------------------------------------*/
void shell_igraph_le_community_to_membership_usage(char **argv) {
  printf("%s --merges=<merges> --steps=<steps> --membership=<membership> --membership-out=<membership-out> --csize=<csize>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_le_community_to_membership(int argc, char **argv) {

  igraph_matrix_t merges;
  igraph_integer_t steps;
  igraph_vector_t membership;
  igraph_vector_t v_csize; igraph_vector_t *csize=0;
  char* shell_arg_membership=0;
  char* shell_arg_csize=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "merges",required_argument,0,0 },
                                   { "steps",required_argument,0,1 },
                                   { "membership",required_argument,0,2 },
                                   { "membership-out",required_argument,0,3 },
                                   { "csize",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* merges */
      shell_read_matrix(&merges, optarg);
      break;
    case 1: /* steps */
      shell_read_integer(&steps, optarg);
      break;
    case 2: /* membership */
      shell_read_vector(&membership, optarg);
      break;
    case 3: /* membership-out */
      shell_arg_membership=strdup(optarg); 
  igraph_vector_init(&membership, 0);
      break;
    case 4: /* csize */
      csize=&v_csize; igraph_vector_init(csize, 0); 
  shell_arg_csize=strdup(optarg);
      break;
    case 5:
      shell_igraph_le_community_to_membership_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_le_community_to_membership(&merges, steps, &membership, csize);

  /* Write the result */
  igraph_matrix_destroy(&merges);
  igraph_vector_destroy(&membership);
  shell_write_vector(&membership, shell_arg_membership); 
  igraph_vector_destroy(&membership);
  if (csize) { shell_write_vector(csize, shell_arg_csize); 
  igraph_vector_destroy(csize); }

  return 0;
}

/*-------------------------------------------/
/ igraph_modularity                          /
/-------------------------------------------*/
void shell_igraph_modularity_usage(char **argv) {
  printf("%s --graph=<graph> --membership=<membership> --modularity=<modularity> --weights=<weights>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_modularity(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t membership;
  igraph_real_t modularity;
  igraph_vector_t v_weights; igraph_vector_t *weights=0=NULL;
  char* shell_arg_modularity=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "membership",required_argument,0,1 },
                                   { "modularity",required_argument,0,2 },
                                   { "weights",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* membership */
      shell_read_vector(&membership, optarg);
      break;
    case 2: /* modularity */
      shell_arg_modularity=strdup(optarg);
      break;
    case 3: /* weights */
      weights=&v_weights; shell_read_vector(weights, optarg);
      break;
    case 4:
      shell_igraph_modularity_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_modularity(&graph, &membership, &modularity, weights);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&membership);
  shell_write_real(modularity, shell_arg_modularity);
  if (weights) { igraph_vector_destroy(weights); }

  return 0;
}

/*-------------------------------------------/
/ igraph_community_leading_eigenvector       /
/-------------------------------------------*/
void shell_igraph_community_leading_eigenvector_usage(char **argv) {
  printf("%s --graph=<graph> --merges=<merges> --membership=<membership> --steps=<steps>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_community_leading_eigenvector(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t merges;
  igraph_vector_t membership;
  igraph_integer_t steps=-1;

  char* shell_arg_merges=0;
  char* shell_arg_membership=0;
  char* shell_arg_options=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "merges",required_argument,0,1 },
                                   { "membership",required_argument,0,2 },
                                   { "steps",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* merges */
      shell_arg_merges=strdup(optarg); 
  igraph_matrix_init(&merges, 0, 0);
      break;
    case 2: /* membership */
      shell_arg_membership=strdup(optarg); 
  igraph_vector_init(&membership, 0);
      break;
    case 3: /* steps */
      shell_read_integer(&steps, optarg);
      break;
    case 4:
      shell_igraph_community_leading_eigenvector_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_community_leading_eigenvector(&graph, &merges, &membership, steps, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&merges, shell_arg_merges); 
  igraph_matrix_destroy(&merges);
  shell_write_vector(&membership, shell_arg_membership); 
  igraph_vector_destroy(&membership);

  return 0;
}

/*-------------------------------------------/
/ igraph_community_leading_eigenvector_naive /
/-------------------------------------------*/
void shell_igraph_community_leading_eigenvector_naive_usage(char **argv) {
  printf("%s --graph=<graph> --merges=<merges> --membership=<membership> --steps=<steps>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_community_leading_eigenvector_naive(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t merges;
  igraph_vector_t membership;
  igraph_integer_t steps=-1;

  char* shell_arg_merges=0;
  char* shell_arg_membership=0;
  char* shell_arg_options=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "merges",required_argument,0,1 },
                                   { "membership",required_argument,0,2 },
                                   { "steps",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* merges */
      shell_arg_merges=strdup(optarg); 
  igraph_matrix_init(&merges, 0, 0);
      break;
    case 2: /* membership */
      shell_arg_membership=strdup(optarg); 
  igraph_vector_init(&membership, 0);
      break;
    case 3: /* steps */
      shell_read_integer(&steps, optarg);
      break;
    case 4:
      shell_igraph_community_leading_eigenvector_naive_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_community_leading_eigenvector_naive(&graph, &merges, &membership, steps, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&merges, shell_arg_merges); 
  igraph_matrix_destroy(&merges);
  shell_write_vector(&membership, shell_arg_membership); 
  igraph_vector_destroy(&membership);

  return 0;
}

/*-------------------------------------------/
/ igraph_community_leading_eigenvector_step  /
/-------------------------------------------*/
void shell_igraph_community_leading_eigenvector_step_usage(char **argv) {
  printf("%s --graph=<graph> --membership=<membership> --membership-out=<membership-out> --community=<community> --split=<split> --eigenvector=<eigenvector> --eigenvalue=<eigenvalue>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_community_leading_eigenvector_step(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t membership;
  igraph_integer_t community;
  igraph_bool_t split;
  igraph_vector_t v_eigenvector; igraph_vector_t *eigenvector=0;
  igraph_real_t eigenvalue;


  char* shell_arg_membership=0;
  char* shell_arg_split=0;
  char* shell_arg_eigenvector=0;
  char* shell_arg_eigenvalue=0;
  char* shell_arg_options=0;
  int shell_result;


  int shell_seen[7];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "membership",required_argument,0,1 },
                                   { "membership-out",required_argument,0,2 },
                                   { "community",required_argument,0,3 },
                                   { "split",required_argument,0,4 },
                                   { "eigenvector",required_argument,0,5 },
                                   { "eigenvalue",required_argument,0,6 },
                                   { "help",no_argument,0,7 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 7*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* membership */
      shell_read_vector(&membership, optarg);
      break;
    case 2: /* membership-out */
      shell_arg_membership=strdup(optarg); 
  igraph_vector_init(&membership, 0);
      break;
    case 3: /* community */
      shell_read_integer(&community, optarg);
      break;
    case 4: /* split */
      
      break;
    case 5: /* eigenvector */
      eigenvector=&v_eigenvector; igraph_vector_init(eigenvector, 0); 
  shell_arg_eigenvector=strdup(optarg);
      break;
    case 6: /* eigenvalue */
      shell_arg_eigenvalue=strdup(optarg);
      break;
    case 7:
      shell_igraph_community_leading_eigenvector_step_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<7; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_community_leading_eigenvector_step(&graph, &membership, community, &split, eigenvector, &eigenvalue, 0, 0);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&membership);
  shell_write_vector(&membership, shell_arg_membership); 
  igraph_vector_destroy(&membership);
  shell_write_boolean(split, shell_arg_split);
  if (eigenvector) { shell_write_vector(eigenvector, shell_arg_eigenvector); 
  igraph_vector_destroy(eigenvector); }
  shell_write_real(eigenvalue, shell_arg_eigenvalue);

  return 0;
}

/*-------------------------------------------/
/ igraph_get_adjacency                       /
/-------------------------------------------*/
void shell_igraph_get_adjacency_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --type=<type>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_get_adjacency(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t res;
  igraph_get_adjacency_t type=IGRAPH_GET_ADJACENCY_BOTH;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "type",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_matrix_init(&res, 0, 0);
      break;
    case 2: /* type */
      shell_read_enum(&type, optarg, "upper", 0, "lower", 1, "both", 2, 0);
      break;
    case 3:
      shell_igraph_get_adjacency_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_get_adjacency(&graph, &res, type);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&res, shell_arg_res); 
  igraph_matrix_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_get_edgelist                        /
/-------------------------------------------*/
void shell_igraph_get_edgelist_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --bycol=<bycol>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_get_edgelist(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  igraph_bool_t bycol=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "bycol",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* bycol */
      shell_read_boolean(&bycol, optarg);
      break;
    case 3:
      shell_igraph_get_edgelist_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_get_edgelist(&graph, &res, bycol);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_to_directed                         /
/-------------------------------------------*/
void shell_igraph_to_directed_usage(char **argv) {
  printf("%s --graph=<graph> --graph-out=<graph-out> --flags=<flags>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_to_directed(int argc, char **argv) {

  igraph_t graph;
  igraph_to_directed_t flags=IGRAPH_TO_DIRECTED_MUTUAL;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "graph-out",required_argument,0,1 },
                                   { "flags",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* graph-out */
      shell_arg_graph=strdup(optarg);
      break;
    case 2: /* flags */
      shell_read_enum(&flags, optarg, "arbitrary", 0, "mutual", 1, 0);
      break;
    case 3:
      shell_igraph_to_directed_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_to_directed(&graph, flags);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_to_undirected                       /
/-------------------------------------------*/
void shell_igraph_to_undirected_usage(char **argv) {
  printf("%s --graph=<graph> --graph-out=<graph-out> --flags=<flags>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_to_undirected(int argc, char **argv) {

  igraph_t graph;
  igraph_to_undirected_t flags=IGRAPH_TO_UNDIRECTED_COLLAPSE;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "graph-out",required_argument,0,1 },
                                   { "flags",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* graph-out */
      shell_arg_graph=strdup(optarg);
      break;
    case 2: /* flags */
      shell_read_enum(&flags, optarg, "each", 0, "collapse", 0);
      break;
    case 3:
      shell_igraph_to_undirected_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_to_undirected(&graph, flags);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_read_graph_edgelist                 /
/-------------------------------------------*/
void shell_igraph_read_graph_edgelist_usage(char **argv) {
  printf("%s --graph=<graph> --instream=<instream> --n=<n> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_read_graph_edgelist(int argc, char **argv) {

  igraph_t graph;
  FILE* instream=0;
  igraph_integer_t n=0;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "instream",required_argument,0,1 },
                                   { "n",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* instream */
      shell_read_file(&instream, optarg, "r");
      break;
    case 2: /* n */
      shell_read_integer(&n, optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4:
      shell_igraph_read_graph_edgelist_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_read_graph_edgelist(&graph, instream, n, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  fclose(instream);

  return 0;
}

/*-------------------------------------------/
/ igraph_read_graph_ncol                     /
/-------------------------------------------*/
void shell_igraph_read_graph_ncol_usage(char **argv) {
  printf("%s --graph=<graph> --instream=<instream> --predefnames=<predefnames> --names=<names> --weights=<weights> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_read_graph_ncol(int argc, char **argv) {

  igraph_t graph;
  FILE* instream=0;
  igraph_strvector_t v_predefnames; igraph_strvector_t *predefnames=0;;
  igraph_bool_t names=1;
  igraph_bool_t weights=1;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "instream",required_argument,0,1 },
                                   { "predefnames",required_argument,0,2 },
                                   { "names",required_argument,0,3 },
                                   { "weights",required_argument,0,4 },
                                   { "directed",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* instream */
      shell_read_file(&instream, optarg, "r");
      break;
    case 2: /* predefnames */
      predefnames=&v_predefnames; shell_read_strvector(predefnames, optarg);
      break;
    case 3: /* names */
      shell_read_boolean(&names, optarg);
      break;
    case 4: /* weights */
      shell_read_boolean(&weights, optarg);
      break;
    case 5: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 6:
      shell_igraph_read_graph_ncol_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_read_graph_ncol(&graph, instream, predefnames, names, weights, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  fclose(instream);
  if (predefnames) { igraph_strvector_destroy(predefnames); }

  return 0;
}

/*-------------------------------------------/
/ igraph_read_graph_lgl                      /
/-------------------------------------------*/
void shell_igraph_read_graph_lgl_usage(char **argv) {
  printf("%s --graph=<graph> --instream=<instream> --names=<names> --weights=<weights>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_read_graph_lgl(int argc, char **argv) {

  igraph_t graph;
  FILE* instream=0;
  igraph_bool_t names=1;
  igraph_bool_t weights=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "instream",required_argument,0,1 },
                                   { "names",required_argument,0,2 },
                                   { "weights",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* instream */
      shell_read_file(&instream, optarg, "r");
      break;
    case 2: /* names */
      shell_read_boolean(&names, optarg);
      break;
    case 3: /* weights */
      shell_read_boolean(&weights, optarg);
      break;
    case 4:
      shell_igraph_read_graph_lgl_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_read_graph_lgl(&graph, instream, names, weights);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  fclose(instream);

  return 0;
}

/*-------------------------------------------/
/ igraph_read_graph_pajek                    /
/-------------------------------------------*/
void shell_igraph_read_graph_pajek_usage(char **argv) {
  printf("%s --graph=<graph> --instream=<instream>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_read_graph_pajek(int argc, char **argv) {

  igraph_t graph;
  FILE* instream=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "instream",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* instream */
      shell_read_file(&instream, optarg, "r");
      break;
    case 2:
      shell_igraph_read_graph_pajek_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_read_graph_pajek(&graph, instream);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  fclose(instream);

  return 0;
}

/*-------------------------------------------/
/ igraph_read_graph_graphml                  /
/-------------------------------------------*/
void shell_igraph_read_graph_graphml_usage(char **argv) {
  printf("%s --graph=<graph> --instream=<instream> --index=<index>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_read_graph_graphml(int argc, char **argv) {

  igraph_t graph;
  FILE* instream=0;
  int index=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "instream",required_argument,0,1 },
                                   { "index",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* instream */
      shell_read_file(&instream, optarg, "r");
      break;
    case 2: /* index */
      shell_read_int(&index, optarg);
      break;
    case 3:
      shell_igraph_read_graph_graphml_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_read_graph_graphml(&graph, instream, index);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  fclose(instream);

  return 0;
}

/*-------------------------------------------/
/ igraph_read_graph_dimacs                   /
/-------------------------------------------*/
void shell_igraph_read_graph_dimacs_usage(char **argv) {
  printf("%s --graph=<graph> --instream=<instream> --source=<source> --target=<target> --capacity=<capacity> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_read_graph_dimacs(int argc, char **argv) {

  igraph_t graph;
  FILE* instream=0;
  igraph_integer_t source;
  igraph_integer_t target;
  igraph_vector_t capacity;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  char* shell_arg_source=0;
  char* shell_arg_target=0;
  char* shell_arg_capacity=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "instream",required_argument,0,1 },
                                   { "source",required_argument,0,2 },
                                   { "target",required_argument,0,3 },
                                   { "capacity",required_argument,0,4 },
                                   { "directed",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* instream */
      shell_read_file(&instream, optarg, "r");
      break;
    case 2: /* source */
      shell_arg_source=strdup(optarg);
      break;
    case 3: /* target */
      shell_arg_target=strdup(optarg);
      break;
    case 4: /* capacity */
      shell_arg_capacity=strdup(optarg); 
  igraph_vector_init(&capacity, 0);
      break;
    case 5: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 6:
      shell_igraph_read_graph_dimacs_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_read_graph_dimacs(&graph, instream, &source, &target, &capacity, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  fclose(instream);
  shell_write_integer(source, shell_arg_source);
  shell_write_integer(target, shell_arg_target);
  shell_write_vector(&capacity, shell_arg_capacity); 
  igraph_vector_destroy(&capacity);

  return 0;
}

/*-------------------------------------------/
/ igraph_read_graph_graphdb                  /
/-------------------------------------------*/
void shell_igraph_read_graph_graphdb_usage(char **argv) {
  printf("%s --graph=<graph> --instream=<instream> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_read_graph_graphdb(int argc, char **argv) {

  igraph_t graph;
  FILE* instream=0;
  igraph_bool_t directed=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "instream",required_argument,0,1 },
                                   { "directed",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* instream */
      shell_read_file(&instream, optarg, "r");
      break;
    case 2: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 3:
      shell_igraph_read_graph_graphdb_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_read_graph_graphdb(&graph, instream, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  fclose(instream);

  return 0;
}

/*-------------------------------------------/
/ igraph_read_graph_gml                      /
/-------------------------------------------*/
void shell_igraph_read_graph_gml_usage(char **argv) {
  printf("%s --graph=<graph> --instream=<instream>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_read_graph_gml(int argc, char **argv) {

  igraph_t graph;
  FILE* instream=0;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "instream",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* instream */
      shell_read_file(&instream, optarg, "r");
      break;
    case 2:
      shell_igraph_read_graph_gml_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_read_graph_gml(&graph, instream);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);
  fclose(instream);

  return 0;
}

/*-------------------------------------------/
/ igraph_write_graph_edgelist                /
/-------------------------------------------*/
void shell_igraph_write_graph_edgelist_usage(char **argv) {
  printf("%s --graph=<graph> --outstream=<outstream>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_write_graph_edgelist(int argc, char **argv) {

  igraph_t graph;
  FILE* outstream=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "outstream",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* outstream */
      shell_read_file(&outstream, optarg, "w");
      break;
    case 2:
      shell_igraph_write_graph_edgelist_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_write_graph_edgelist(&graph, outstream);

  /* Write the result */
  igraph_destroy(&graph);
  fclose(outstream);

  return 0;
}

/*-------------------------------------------/
/ igraph_write_graph_ncol                    /
/-------------------------------------------*/
void shell_igraph_write_graph_ncol_usage(char **argv) {
  printf("%s --graph=<graph> --outstream=<outstream> --names=<names> --weights=<weights>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_write_graph_ncol(int argc, char **argv) {

  igraph_t graph;
  FILE* outstream=0;
  char * names="name";
  char * weights="weight";
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "outstream",required_argument,0,1 },
                                   { "names",required_argument,0,2 },
                                   { "weights",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* outstream */
      shell_read_file(&outstream, optarg, "w");
      break;
    case 2: /* names */
      names=strdup(optarg);
      break;
    case 3: /* weights */
      weights=strdup(optarg);
      break;
    case 4:
      shell_igraph_write_graph_ncol_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_write_graph_ncol(&graph, outstream, names, weights);

  /* Write the result */
  igraph_destroy(&graph);
  fclose(outstream);
  free(names);
  free(weights);

  return 0;
}

/*-------------------------------------------/
/ igraph_write_graph_lgl                     /
/-------------------------------------------*/
void shell_igraph_write_graph_lgl_usage(char **argv) {
  printf("%s --graph=<graph> --outstream=<outstream> --names=<names> --weights=<weights> --isolates=<isolates>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_write_graph_lgl(int argc, char **argv) {

  igraph_t graph;
  FILE* outstream=0;
  char * names="name";
  char * weights="weight";
  igraph_bool_t isolates=1;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "outstream",required_argument,0,1 },
                                   { "names",required_argument,0,2 },
                                   { "weights",required_argument,0,3 },
                                   { "isolates",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* outstream */
      shell_read_file(&outstream, optarg, "w");
      break;
    case 2: /* names */
      names=strdup(optarg);
      break;
    case 3: /* weights */
      weights=strdup(optarg);
      break;
    case 4: /* isolates */
      shell_read_boolean(&isolates, optarg);
      break;
    case 5:
      shell_igraph_write_graph_lgl_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_write_graph_lgl(&graph, outstream, names, weights, isolates);

  /* Write the result */
  igraph_destroy(&graph);
  fclose(outstream);
  free(names);
  free(weights);

  return 0;
}

/*-------------------------------------------/
/ igraph_write_graph_graphml                 /
/-------------------------------------------*/
void shell_igraph_write_graph_graphml_usage(char **argv) {
  printf("%s --graph=<graph> --outstream=<outstream>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_write_graph_graphml(int argc, char **argv) {

  igraph_t graph;
  FILE* outstream=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "outstream",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* outstream */
      shell_read_file(&outstream, optarg, "w");
      break;
    case 2:
      shell_igraph_write_graph_graphml_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_write_graph_graphml(&graph, outstream);

  /* Write the result */
  igraph_destroy(&graph);
  fclose(outstream);

  return 0;
}

/*-------------------------------------------/
/ igraph_write_graph_pajek                   /
/-------------------------------------------*/
void shell_igraph_write_graph_pajek_usage(char **argv) {
  printf("%s --graph=<graph> --outstream=<outstream>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_write_graph_pajek(int argc, char **argv) {

  igraph_t graph;
  FILE* outstream=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "outstream",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* outstream */
      shell_read_file(&outstream, optarg, "w");
      break;
    case 2:
      shell_igraph_write_graph_pajek_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_write_graph_pajek(&graph, outstream);

  /* Write the result */
  igraph_destroy(&graph);
  fclose(outstream);

  return 0;
}

/*-------------------------------------------/
/ igraph_write_graph_dimacs                  /
/-------------------------------------------*/
void shell_igraph_write_graph_dimacs_usage(char **argv) {
  printf("%s --graph=<graph> --outstream=<outstream> --source=<source> --target=<target> --capacity=<capacity>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_write_graph_dimacs(int argc, char **argv) {

  igraph_t graph;
  FILE* outstream=0;
  long int source=0;
  long int target=0;
  igraph_vector_t capacity;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "outstream",required_argument,0,1 },
                                   { "source",required_argument,0,2 },
                                   { "target",required_argument,0,3 },
                                   { "capacity",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[2]=2;
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* outstream */
      shell_read_file(&outstream, optarg, "w");
      break;
    case 2: /* source */
      shell_read_longint(&source, optarg);
      break;
    case 3: /* target */
      shell_read_longint(&target, optarg);
      break;
    case 4: /* capacity */
      shell_read_vector(&capacity, optarg);
      break;
    case 5:
      shell_igraph_write_graph_dimacs_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_write_graph_dimacs(&graph, outstream, source, target, &capacity);

  /* Write the result */
  igraph_destroy(&graph);
  fclose(outstream);
  igraph_vector_destroy(&capacity);

  return 0;
}

/*-------------------------------------------/
/ igraph_write_graph_gml                     /
/-------------------------------------------*/
void shell_igraph_write_graph_gml_usage(char **argv) {
  printf("%s --graph=<graph> --outstream=<outstream> --id=<id> --creator=<creator>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_write_graph_gml(int argc, char **argv) {

  igraph_t graph;
  FILE* outstream=0;
  igraph_vector_t id;
  char * creator="igraph";
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "outstream",required_argument,0,1 },
                                   { "id",required_argument,0,2 },
                                   { "creator",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* outstream */
      shell_read_file(&outstream, optarg, "w");
      break;
    case 2: /* id */
      shell_read_vector(&id, optarg);
      break;
    case 3: /* creator */
      creator=strdup(optarg);
      break;
    case 4:
      shell_igraph_write_graph_gml_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_write_graph_gml(&graph, outstream, &id, creator);

  /* Write the result */
  igraph_destroy(&graph);
  fclose(outstream);
  igraph_vector_destroy(&id);
  free(creator);

  return 0;
}

/*-------------------------------------------/
/ igraph_write_graph_dot                     /
/-------------------------------------------*/
void shell_igraph_write_graph_dot_usage(char **argv) {
  printf("%s --graph=<graph> --outstream=<outstream>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_write_graph_dot(int argc, char **argv) {

  igraph_t graph;
  FILE* outstream=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "outstream",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* outstream */
      shell_read_file(&outstream, optarg, "w");
      break;
    case 2:
      shell_igraph_write_graph_dot_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_write_graph_dot(&graph, outstream);

  /* Write the result */
  igraph_destroy(&graph);
  fclose(outstream);

  return 0;
}

/*-------------------------------------------/
/ igraph_motifs_randesu                      /
/-------------------------------------------*/
void shell_igraph_motifs_randesu_usage(char **argv) {
  printf("%s --graph=<graph> --hist=<hist> --size=<size> --cut_prob=<cut_prob>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_motifs_randesu(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t hist;
  int size=3;
  igraph_vector_t cut_prob;
  char* shell_arg_hist=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "hist",required_argument,0,1 },
                                   { "size",required_argument,0,2 },
                                   { "cut_prob",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* hist */
      shell_arg_hist=strdup(optarg); 
  igraph_vector_init(&hist, 0);
      break;
    case 2: /* size */
      shell_read_int(&size, optarg);
      break;
    case 3: /* cut_prob */
      shell_read_vector(&cut_prob, optarg);
      break;
    case 4:
      shell_igraph_motifs_randesu_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_motifs_randesu(&graph, &hist, size, &cut_prob);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&hist, shell_arg_hist); 
  igraph_vector_destroy(&hist);
  igraph_vector_destroy(&cut_prob);

  return 0;
}

/*-------------------------------------------/
/ igraph_motifs_randesu_estimate             /
/-------------------------------------------*/
void shell_igraph_motifs_randesu_estimate_usage(char **argv) {
  printf("%s --graph=<graph> --est=<est> --size=<size> --cut_prob=<cut_prob> --sample_size=<sample_size> --sample=<sample>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_motifs_randesu_estimate(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t est;
  int size=3;
  igraph_vector_t cut_prob;
  igraph_integer_t sample_size;
  igraph_vector_t v_sample; igraph_vector_t *sample=0;
  char* shell_arg_est=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "est",required_argument,0,1 },
                                   { "size",required_argument,0,2 },
                                   { "cut_prob",required_argument,0,3 },
                                   { "sample_size",required_argument,0,4 },
                                   { "sample",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* est */
      shell_arg_est=strdup(optarg);
      break;
    case 2: /* size */
      shell_read_int(&size, optarg);
      break;
    case 3: /* cut_prob */
      shell_read_vector(&cut_prob, optarg);
      break;
    case 4: /* sample_size */
      shell_read_integer(&sample_size, optarg);
      break;
    case 5: /* sample */
      sample=&v_sample; shell_read_vector(sample, optarg);
      break;
    case 6:
      shell_igraph_motifs_randesu_estimate_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_motifs_randesu_estimate(&graph, &est, size, &cut_prob, sample_size, sample);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(est, shell_arg_est);
  igraph_vector_destroy(&cut_prob);
  if (sample) { igraph_vector_destroy(sample); }

  return 0;
}

/*-------------------------------------------/
/ igraph_motifs_randesu_no                   /
/-------------------------------------------*/
void shell_igraph_motifs_randesu_no_usage(char **argv) {
  printf("%s --graph=<graph> --no=<no> --size=<size> --cut_prob=<cut_prob>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_motifs_randesu_no(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t no;
  int size=3;
  igraph_vector_t cut_prob;
  char* shell_arg_no=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "no",required_argument,0,1 },
                                   { "size",required_argument,0,2 },
                                   { "cut_prob",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* no */
      shell_arg_no=strdup(optarg);
      break;
    case 2: /* size */
      shell_read_int(&size, optarg);
      break;
    case 3: /* cut_prob */
      shell_read_vector(&cut_prob, optarg);
      break;
    case 4:
      shell_igraph_motifs_randesu_no_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_motifs_randesu_no(&graph, &no, size, &cut_prob);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(no, shell_arg_no);
  igraph_vector_destroy(&cut_prob);

  return 0;
}

/*-------------------------------------------/
/ igraph_dyad_census                         /
/-------------------------------------------*/
void shell_igraph_dyad_census_usage(char **argv) {
  printf("%s --graph=<graph> --mut=<mut> --asym=<asym> --null=<null>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_dyad_census(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t mut;
  igraph_integer_t asym;
  igraph_integer_t null;
  char* shell_arg_mut=0;
  char* shell_arg_asym=0;
  char* shell_arg_null=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "mut",required_argument,0,1 },
                                   { "asym",required_argument,0,2 },
                                   { "null",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* mut */
      shell_arg_mut=strdup(optarg);
      break;
    case 2: /* asym */
      shell_arg_asym=strdup(optarg);
      break;
    case 3: /* null */
      shell_arg_null=strdup(optarg);
      break;
    case 4:
      shell_igraph_dyad_census_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_dyad_census(&graph, &mut, &asym, &null);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(mut, shell_arg_mut);
  shell_write_integer(asym, shell_arg_asym);
  shell_write_integer(null, shell_arg_null);

  return 0;
}

/*-------------------------------------------/
/ igraph_triad_census                        /
/-------------------------------------------*/
void shell_igraph_triad_census_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_triad_census(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t res;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2:
      shell_igraph_triad_census_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_triad_census(&graph, &res);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_disjoint_union                      /
/-------------------------------------------*/
void shell_igraph_disjoint_union_usage(char **argv) {
  printf("%s --res=<res> --left=<left> --right=<right>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_disjoint_union(int argc, char **argv) {

  igraph_t res;
  igraph_t left;
  igraph_t right;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "res",required_argument,0,0 },
                                   { "left",required_argument,0,1 },
                                   { "right",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 1: /* left */
      shell_read_graph(&left, optarg);
      break;
    case 2: /* right */
      shell_read_graph(&right, optarg);
      break;
    case 3:
      shell_igraph_disjoint_union_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_disjoint_union(&res, &left, &right);

  /* Write the result */
  shell_write_graph(&res, shell_arg_res); 
  igraph_destroy(&res);
  igraph_destroy(&left);
  igraph_destroy(&right);

  return 0;
}

/*-------------------------------------------/
/ igraph_disjoint_union_many                 /
/-------------------------------------------*/
void shell_igraph_disjoint_union_many_usage(char **argv) {
  printf("%s --res=<res> --graphs=<graphs>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_disjoint_union_many(int argc, char **argv) {

  igraph_t res;
  igraph_vector_ptr_t graphs;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "res",required_argument,0,0 },
                                   { "graphs",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 1: /* graphs */
      shell_read_graphlist(&graphs, optarg);
      break;
    case 2:
      shell_igraph_disjoint_union_many_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_disjoint_union_many(&res, &graphs);

  /* Write the result */
  shell_write_graph(&res, shell_arg_res); 
  igraph_destroy(&res);
  shell_free_graphlist(&graphs); 
  igraph_vector_ptr_destroy(&graphs);

  return 0;
}

/*-------------------------------------------/
/ igraph_union                               /
/-------------------------------------------*/
void shell_igraph_union_usage(char **argv) {
  printf("%s --res=<res> --left=<left> --right=<right>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_union(int argc, char **argv) {

  igraph_t res;
  igraph_t left;
  igraph_t right;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "res",required_argument,0,0 },
                                   { "left",required_argument,0,1 },
                                   { "right",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 1: /* left */
      shell_read_graph(&left, optarg);
      break;
    case 2: /* right */
      shell_read_graph(&right, optarg);
      break;
    case 3:
      shell_igraph_union_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_union(&res, &left, &right);

  /* Write the result */
  shell_write_graph(&res, shell_arg_res); 
  igraph_destroy(&res);
  igraph_destroy(&left);
  igraph_destroy(&right);

  return 0;
}

/*-------------------------------------------/
/ igraph_union_many                          /
/-------------------------------------------*/
void shell_igraph_union_many_usage(char **argv) {
  printf("%s --res=<res> --graphs=<graphs>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_union_many(int argc, char **argv) {

  igraph_t res;
  igraph_vector_ptr_t graphs;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "res",required_argument,0,0 },
                                   { "graphs",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 1: /* graphs */
      shell_read_graphlist(&graphs, optarg);
      break;
    case 2:
      shell_igraph_union_many_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_union_many(&res, &graphs);

  /* Write the result */
  shell_write_graph(&res, shell_arg_res); 
  igraph_destroy(&res);
  shell_free_graphlist(&graphs); 
  igraph_vector_ptr_destroy(&graphs);

  return 0;
}

/*-------------------------------------------/
/ igraph_intersection                        /
/-------------------------------------------*/
void shell_igraph_intersection_usage(char **argv) {
  printf("%s --res=<res> --left=<left> --right=<right>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_intersection(int argc, char **argv) {

  igraph_t res;
  igraph_t left;
  igraph_t right;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "res",required_argument,0,0 },
                                   { "left",required_argument,0,1 },
                                   { "right",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 1: /* left */
      shell_read_graph(&left, optarg);
      break;
    case 2: /* right */
      shell_read_graph(&right, optarg);
      break;
    case 3:
      shell_igraph_intersection_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_intersection(&res, &left, &right);

  /* Write the result */
  shell_write_graph(&res, shell_arg_res); 
  igraph_destroy(&res);
  igraph_destroy(&left);
  igraph_destroy(&right);

  return 0;
}

/*-------------------------------------------/
/ igraph_intersection_many                   /
/-------------------------------------------*/
void shell_igraph_intersection_many_usage(char **argv) {
  printf("%s --res=<res> --graphs=<graphs>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_intersection_many(int argc, char **argv) {

  igraph_t res;
  igraph_vector_ptr_t graphs;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "res",required_argument,0,0 },
                                   { "graphs",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 1: /* graphs */
      shell_read_graphlist(&graphs, optarg);
      break;
    case 2:
      shell_igraph_intersection_many_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_intersection_many(&res, &graphs);

  /* Write the result */
  shell_write_graph(&res, shell_arg_res); 
  igraph_destroy(&res);
  shell_free_graphlist(&graphs); 
  igraph_vector_ptr_destroy(&graphs);

  return 0;
}

/*-------------------------------------------/
/ igraph_difference                          /
/-------------------------------------------*/
void shell_igraph_difference_usage(char **argv) {
  printf("%s --res=<res> --orig=<orig> --sub=<sub>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_difference(int argc, char **argv) {

  igraph_t res;
  igraph_t orig;
  igraph_t sub;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "res",required_argument,0,0 },
                                   { "orig",required_argument,0,1 },
                                   { "sub",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 1: /* orig */
      shell_read_graph(&orig, optarg);
      break;
    case 2: /* sub */
      shell_read_graph(&sub, optarg);
      break;
    case 3:
      shell_igraph_difference_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_difference(&res, &orig, &sub);

  /* Write the result */
  shell_write_graph(&res, shell_arg_res); 
  igraph_destroy(&res);
  igraph_destroy(&orig);
  igraph_destroy(&sub);

  return 0;
}

/*-------------------------------------------/
/ igraph_complementer                        /
/-------------------------------------------*/
void shell_igraph_complementer_usage(char **argv) {
  printf("%s --res=<res> --graph=<graph> --loops=<loops>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_complementer(int argc, char **argv) {

  igraph_t res;
  igraph_t graph;
  igraph_bool_t loops=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "res",required_argument,0,0 },
                                   { "graph",required_argument,0,1 },
                                   { "loops",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 1: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 2: /* loops */
      shell_read_boolean(&loops, optarg);
      break;
    case 3:
      shell_igraph_complementer_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_complementer(&res, &graph, loops);

  /* Write the result */
  shell_write_graph(&res, shell_arg_res); 
  igraph_destroy(&res);
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_compose                             /
/-------------------------------------------*/
void shell_igraph_compose_usage(char **argv) {
  printf("%s --res=<res> --g1=<g1> --g2=<g2>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_compose(int argc, char **argv) {

  igraph_t res;
  igraph_t g1;
  igraph_t g2;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "res",required_argument,0,0 },
                                   { "g1",required_argument,0,1 },
                                   { "g2",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 1: /* g1 */
      shell_read_graph(&g1, optarg);
      break;
    case 2: /* g2 */
      shell_read_graph(&g2, optarg);
      break;
    case 3:
      shell_igraph_compose_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_compose(&res, &g1, &g2);

  /* Write the result */
  shell_write_graph(&res, shell_arg_res); 
  igraph_destroy(&res);
  igraph_destroy(&g1);
  igraph_destroy(&g2);

  return 0;
}

/*-------------------------------------------/
/ igraph_maxflow_value                       /
/-------------------------------------------*/
void shell_igraph_maxflow_value_usage(char **argv) {
  printf("%s --graph=<graph> --value=<value> --source=<source> --target=<target> --capacity=<capacity>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_maxflow_value(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t value;
  igraph_integer_t source;
  igraph_integer_t target;
  igraph_vector_t v_capacity; igraph_vector_t *capacity=0;
  char* shell_arg_value=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "value",required_argument,0,1 },
                                   { "source",required_argument,0,2 },
                                   { "target",required_argument,0,3 },
                                   { "capacity",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* value */
      shell_arg_value=strdup(optarg);
      break;
    case 2: /* source */
      shell_read_integer(&source, optarg);
      break;
    case 3: /* target */
      shell_read_integer(&target, optarg);
      break;
    case 4: /* capacity */
      capacity=&v_capacity; shell_read_vector(capacity, optarg);
      break;
    case 5:
      shell_igraph_maxflow_value_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_maxflow_value(&graph, &value, source, target, capacity);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(value, shell_arg_value);
  if (capacity) { igraph_vector_destroy(capacity); }

  return 0;
}

/*-------------------------------------------/
/ igraph_mincut_value                        /
/-------------------------------------------*/
void shell_igraph_mincut_value_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --capacity=<capacity>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_mincut_value(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t res;
  igraph_vector_t v_capacity; igraph_vector_t *capacity=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "capacity",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* capacity */
      capacity=&v_capacity; shell_read_vector(capacity, optarg);
      break;
    case 3:
      shell_igraph_mincut_value_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_mincut_value(&graph, &res, capacity);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(res, shell_arg_res);
  if (capacity) { igraph_vector_destroy(capacity); }

  return 0;
}

/*-------------------------------------------/
/ igraph_st_mincut_value                     /
/-------------------------------------------*/
void shell_igraph_st_mincut_value_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --source=<source> --target=<target> --capacity=<capacity>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_st_mincut_value(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t res;
  igraph_integer_t source;
  igraph_integer_t target;
  igraph_vector_t v_capacity; igraph_vector_t *capacity=0;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "source",required_argument,0,2 },
                                   { "target",required_argument,0,3 },
                                   { "capacity",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* source */
      shell_read_integer(&source, optarg);
      break;
    case 3: /* target */
      shell_read_integer(&target, optarg);
      break;
    case 4: /* capacity */
      capacity=&v_capacity; shell_read_vector(capacity, optarg);
      break;
    case 5:
      shell_igraph_st_mincut_value_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_st_mincut_value(&graph, &res, source, target, capacity);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(res, shell_arg_res);
  if (capacity) { igraph_vector_destroy(capacity); }

  return 0;
}

/*-------------------------------------------/
/ igraph_mincut                              /
/-------------------------------------------*/
void shell_igraph_mincut_usage(char **argv) {
  printf("%s --graph=<graph> --value=<value> --partition=<partition> --partition2=<partition2> --cut=<cut> --capacity=<capacity>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_mincut(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t value;
  igraph_vector_t partition;
  igraph_vector_t partition2;
  igraph_vector_t cut;
  igraph_vector_t v_capacity; igraph_vector_t *capacity=0;
  char* shell_arg_value=0;
  char* shell_arg_partition=0;
  char* shell_arg_partition2=0;
  char* shell_arg_cut=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "value",required_argument,0,1 },
                                   { "partition",required_argument,0,2 },
                                   { "partition2",required_argument,0,3 },
                                   { "cut",required_argument,0,4 },
                                   { "capacity",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* value */
      shell_arg_value=strdup(optarg);
      break;
    case 2: /* partition */
      shell_arg_partition=strdup(optarg); 
  igraph_vector_init(&partition, 0);
      break;
    case 3: /* partition2 */
      shell_arg_partition2=strdup(optarg); 
  igraph_vector_init(&partition2, 0);
      break;
    case 4: /* cut */
      shell_arg_cut=strdup(optarg); 
  igraph_vector_init(&cut, 0);
      break;
    case 5: /* capacity */
      capacity=&v_capacity; shell_read_vector(capacity, optarg);
      break;
    case 6:
      shell_igraph_mincut_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_mincut(&graph, &value, &partition, &partition2, &cut, capacity);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(value, shell_arg_value);
  shell_write_vector(&partition, shell_arg_partition); 
  igraph_vector_destroy(&partition);
  shell_write_vector(&partition2, shell_arg_partition2); 
  igraph_vector_destroy(&partition2);
  shell_write_vector(&cut, shell_arg_cut); 
  igraph_vector_destroy(&cut);
  if (capacity) { igraph_vector_destroy(capacity); }

  return 0;
}

/*-------------------------------------------/
/ igraph_st_vertex_connectivity              /
/-------------------------------------------*/
void shell_igraph_st_vertex_connectivity_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --source=<source> --target=<target> --neighbors=<neighbors>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_st_vertex_connectivity(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t res;
  igraph_integer_t source;
  igraph_integer_t target;
  igraph_vconn_nei_t neighbors=IGRAPH_VCONN_NEI_INFINITY;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "source",required_argument,0,2 },
                                   { "target",required_argument,0,3 },
                                   { "neighbors",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* source */
      shell_read_integer(&source, optarg);
      break;
    case 3: /* target */
      shell_read_integer(&target, optarg);
      break;
    case 4: /* neighbors */
      shell_read_enum(&neighbors, optarg, "error", 0, "infinity", 1, "ignore", 2, 0);
      break;
    case 5:
      shell_igraph_st_vertex_connectivity_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_st_vertex_connectivity(&graph, &res, source, target, neighbors);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_vertex_connectivity                 /
/-------------------------------------------*/
void shell_igraph_vertex_connectivity_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --checks=<checks>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_vertex_connectivity(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t res;
  igraph_bool_t checks=1;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "checks",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* checks */
      shell_read_boolean(&checks, optarg);
      break;
    case 3:
      shell_igraph_vertex_connectivity_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_vertex_connectivity(&graph, &res, checks);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_st_edge_connectivity                /
/-------------------------------------------*/
void shell_igraph_st_edge_connectivity_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --source=<source> --target=<target>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_st_edge_connectivity(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t res;
  igraph_integer_t source;
  igraph_integer_t target;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "source",required_argument,0,2 },
                                   { "target",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* source */
      shell_read_integer(&source, optarg);
      break;
    case 3: /* target */
      shell_read_integer(&target, optarg);
      break;
    case 4:
      shell_igraph_st_edge_connectivity_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_st_edge_connectivity(&graph, &res, source, target);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_edge_connectivity                   /
/-------------------------------------------*/
void shell_igraph_edge_connectivity_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --checks=<checks>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_edge_connectivity(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t res;
  igraph_bool_t checks=1;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "checks",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* checks */
      shell_read_boolean(&checks, optarg);
      break;
    case 3:
      shell_igraph_edge_connectivity_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_edge_connectivity(&graph, &res, checks);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_edge_disjoint_paths                 /
/-------------------------------------------*/
void shell_igraph_edge_disjoint_paths_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --source=<source> --target=<target>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_edge_disjoint_paths(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t res;
  igraph_integer_t source;
  igraph_integer_t target;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "source",required_argument,0,2 },
                                   { "target",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* source */
      shell_read_integer(&source, optarg);
      break;
    case 3: /* target */
      shell_read_integer(&target, optarg);
      break;
    case 4:
      shell_igraph_edge_disjoint_paths_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_edge_disjoint_paths(&graph, &res, source, target);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_vertex_disjoint_paths               /
/-------------------------------------------*/
void shell_igraph_vertex_disjoint_paths_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --source=<source> --target=<target>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_vertex_disjoint_paths(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t res;
  igraph_integer_t source;
  igraph_integer_t target;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "source",required_argument,0,2 },
                                   { "target",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* source */
      shell_read_integer(&source, optarg);
      break;
    case 3: /* target */
      shell_read_integer(&target, optarg);
      break;
    case 4:
      shell_igraph_vertex_disjoint_paths_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_vertex_disjoint_paths(&graph, &res, source, target);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_adhesion                            /
/-------------------------------------------*/
void shell_igraph_adhesion_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --checks=<checks>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_adhesion(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t res;
  igraph_bool_t checks=1;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "checks",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* checks */
      shell_read_boolean(&checks, optarg);
      break;
    case 3:
      shell_igraph_adhesion_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_adhesion(&graph, &res, checks);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_cohesion                            /
/-------------------------------------------*/
void shell_igraph_cohesion_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --checks=<checks>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_cohesion(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t res;
  igraph_bool_t checks=1;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "checks",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* checks */
      shell_read_boolean(&checks, optarg);
      break;
    case 3:
      shell_igraph_cohesion_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_cohesion(&graph, &res, checks);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(res, shell_arg_res);

  return 0;
}

/*-------------------------------------------/
/ igraph_coreness                            /
/-------------------------------------------*/
void shell_igraph_coreness_usage(char **argv) {
  printf("%s --graph=<graph> --cores=<cores> --mode=<mode>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_coreness(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t cores;
  igraph_neimode_t mode=IGRAPH_ALL;
  char* shell_arg_cores=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "cores",required_argument,0,1 },
                                   { "mode",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));
  shell_seen[2]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* cores */
      shell_arg_cores=strdup(optarg); 
  igraph_vector_init(&cores, 0);
      break;
    case 2: /* mode */
      shell_read_enum(&mode, optarg, "out", 1, "in", 2, "all", 3, "total", 3, 0);
      break;
    case 3:
      shell_igraph_coreness_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_coreness(&graph, &cores, mode);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&cores, shell_arg_cores); 
  igraph_vector_destroy(&cores);

  return 0;
}

/*-------------------------------------------/
/ igraph_isoclass                            /
/-------------------------------------------*/
void shell_igraph_isoclass_usage(char **argv) {
  printf("%s --graph=<graph> --isoclass=<isoclass>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_isoclass(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t isoclass;
  char* shell_arg_isoclass=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "isoclass",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* isoclass */
      shell_arg_isoclass=strdup(optarg);
      break;
    case 2:
      shell_igraph_isoclass_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_isoclass(&graph, &isoclass);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_integer(isoclass, shell_arg_isoclass);

  return 0;
}

/*-------------------------------------------/
/ igraph_isomorphic                          /
/-------------------------------------------*/
void shell_igraph_isomorphic_usage(char **argv) {
  printf("%s --graph1=<graph1> --graph2=<graph2> --iso=<iso>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_isomorphic(int argc, char **argv) {

  igraph_t graph1;
  igraph_t graph2;
  igraph_bool_t iso;
  char* shell_arg_iso=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph1",required_argument,0,0 },
                                   { "graph2",required_argument,0,1 },
                                   { "iso",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph1 */
      shell_read_graph(&graph1, optarg);
      break;
    case 1: /* graph2 */
      shell_read_graph(&graph2, optarg);
      break;
    case 2: /* iso */
      
      break;
    case 3:
      shell_igraph_isomorphic_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_isomorphic(&graph1, &graph2, &iso);

  /* Write the result */
  igraph_destroy(&graph1);
  igraph_destroy(&graph2);
  shell_write_boolean(iso, shell_arg_iso);

  return 0;
}

/*-------------------------------------------/
/ igraph_isoclass_subgraph                   /
/-------------------------------------------*/
void shell_igraph_isoclass_subgraph_usage(char **argv) {
  printf("%s --graph=<graph> --vids=<vids> --isoclass=<isoclass>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_isoclass_subgraph(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t vids;
  igraph_integer_t isoclass;
  char* shell_arg_isoclass=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "vids",required_argument,0,1 },
                                   { "isoclass",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* vids */
      shell_read_vector(&vids, optarg);
      break;
    case 2: /* isoclass */
      shell_arg_isoclass=strdup(optarg);
      break;
    case 3:
      shell_igraph_isoclass_subgraph_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_isoclass_subgraph(&graph, &vids, &isoclass);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&vids);
  shell_write_integer(isoclass, shell_arg_isoclass);

  return 0;
}

/*-------------------------------------------/
/ igraph_isoclass_create                     /
/-------------------------------------------*/
void shell_igraph_isoclass_create_usage(char **argv) {
  printf("%s --graph=<graph> --size=<size> --number=<number> --directed=<directed>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_isoclass_create(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t size;
  igraph_integer_t number;
  igraph_bool_t directed=1;
  char* shell_arg_graph=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "size",required_argument,0,1 },
                                   { "number",required_argument,0,2 },
                                   { "directed",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_arg_graph=strdup(optarg);
      break;
    case 1: /* size */
      shell_read_integer(&size, optarg);
      break;
    case 2: /* number */
      shell_read_integer(&number, optarg);
      break;
    case 3: /* directed */
      shell_read_boolean(&directed, optarg);
      break;
    case 4:
      shell_igraph_isoclass_create_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_isoclass_create(&graph, size, number, directed);

  /* Write the result */
  shell_write_graph(&graph, shell_arg_graph); 
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_isomorphic_vf2                      /
/-------------------------------------------*/
void shell_igraph_isomorphic_vf2_usage(char **argv) {
  printf("%s --graph1=<graph1> --graph2=<graph2> --iso=<iso> --map12=<map12> --map21=<map21>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_isomorphic_vf2(int argc, char **argv) {

  igraph_t graph1;
  igraph_t graph2;
  igraph_bool_t iso;
  igraph_vector_t v_map12; igraph_vector_t *map12=0;
  igraph_vector_t v_map21; igraph_vector_t *map21=0;
  char* shell_arg_iso=0;
  char* shell_arg_map12=0;
  char* shell_arg_map21=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph1",required_argument,0,0 },
                                   { "graph2",required_argument,0,1 },
                                   { "iso",required_argument,0,2 },
                                   { "map12",required_argument,0,3 },
                                   { "map21",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph1 */
      shell_read_graph(&graph1, optarg);
      break;
    case 1: /* graph2 */
      shell_read_graph(&graph2, optarg);
      break;
    case 2: /* iso */
      
      break;
    case 3: /* map12 */
      map12=&v_map12; igraph_vector_init(map12, 0); 
  shell_arg_map12=strdup(optarg);
      break;
    case 4: /* map21 */
      map21=&v_map21; igraph_vector_init(map21, 0); 
  shell_arg_map21=strdup(optarg);
      break;
    case 5:
      shell_igraph_isomorphic_vf2_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_isomorphic_vf2(&graph1, &graph2, &iso, map12, map21);

  /* Write the result */
  igraph_destroy(&graph1);
  igraph_destroy(&graph2);
  shell_write_boolean(iso, shell_arg_iso);
  if (map12) { shell_write_vector(map12, shell_arg_map12); 
  igraph_vector_destroy(map12); }
  if (map21) { shell_write_vector(map21, shell_arg_map21); 
  igraph_vector_destroy(map21); }

  return 0;
}

/*-------------------------------------------/
/ igraph_count_isomorphisms_vf2              /
/-------------------------------------------*/
void shell_igraph_count_isomorphisms_vf2_usage(char **argv) {
  printf("%s --graph1=<graph1> --graph2=<graph2> --count=<count>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_count_isomorphisms_vf2(int argc, char **argv) {

  igraph_t graph1;
  igraph_t graph2;
  igraph_integer_t count;
  char* shell_arg_count=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph1",required_argument,0,0 },
                                   { "graph2",required_argument,0,1 },
                                   { "count",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph1 */
      shell_read_graph(&graph1, optarg);
      break;
    case 1: /* graph2 */
      shell_read_graph(&graph2, optarg);
      break;
    case 2: /* count */
      shell_arg_count=strdup(optarg);
      break;
    case 3:
      shell_igraph_count_isomorphisms_vf2_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_count_isomorphisms_vf2(&graph1, &graph2, &count);

  /* Write the result */
  igraph_destroy(&graph1);
  igraph_destroy(&graph2);
  shell_write_integer(count, shell_arg_count);

  return 0;
}

/*-------------------------------------------/
/ igraph_get_isomorphisms_vf2                /
/-------------------------------------------*/
void shell_igraph_get_isomorphisms_vf2_usage(char **argv) {
  printf("%s --graph1=<graph1> --graph2=<graph2> --maps=<maps>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_get_isomorphisms_vf2(int argc, char **argv) {

  igraph_t graph1;
  igraph_t graph2;
  igraph_vector_ptr_t maps;
  char* shell_arg_maps=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph1",required_argument,0,0 },
                                   { "graph2",required_argument,0,1 },
                                   { "maps",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph1 */
      shell_read_graph(&graph1, optarg);
      break;
    case 1: /* graph2 */
      shell_read_graph(&graph2, optarg);
      break;
    case 2: /* maps */
      igraph_vector_ptr_init(&maps, 0);
      break;
    case 3:
      shell_igraph_get_isomorphisms_vf2_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_get_isomorphisms_vf2(&graph1, &graph2, &maps);

  /* Write the result */
  igraph_destroy(&graph1);
  igraph_destroy(&graph2);
  shell_write_vectorlist(&maps, shell_arg_maps); 
  shell_free_vectorlist(&maps); 
  igraph_vector_ptr_destroy(&maps);

  return 0;
}

/*-------------------------------------------/
/ igraph_subisomorphic_vf2                   /
/-------------------------------------------*/
void shell_igraph_subisomorphic_vf2_usage(char **argv) {
  printf("%s --graph1=<graph1> --graph2=<graph2> --iso=<iso> --map12=<map12> --map21=<map21>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_subisomorphic_vf2(int argc, char **argv) {

  igraph_t graph1;
  igraph_t graph2;
  igraph_bool_t iso;
  igraph_vector_t v_map12; igraph_vector_t *map12=0;
  igraph_vector_t v_map21; igraph_vector_t *map21=0;
  char* shell_arg_iso=0;
  char* shell_arg_map12=0;
  char* shell_arg_map21=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph1",required_argument,0,0 },
                                   { "graph2",required_argument,0,1 },
                                   { "iso",required_argument,0,2 },
                                   { "map12",required_argument,0,3 },
                                   { "map21",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph1 */
      shell_read_graph(&graph1, optarg);
      break;
    case 1: /* graph2 */
      shell_read_graph(&graph2, optarg);
      break;
    case 2: /* iso */
      
      break;
    case 3: /* map12 */
      map12=&v_map12; igraph_vector_init(map12, 0); 
  shell_arg_map12=strdup(optarg);
      break;
    case 4: /* map21 */
      map21=&v_map21; igraph_vector_init(map21, 0); 
  shell_arg_map21=strdup(optarg);
      break;
    case 5:
      shell_igraph_subisomorphic_vf2_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_subisomorphic_vf2(&graph1, &graph2, &iso, map12, map21);

  /* Write the result */
  igraph_destroy(&graph1);
  igraph_destroy(&graph2);
  shell_write_boolean(iso, shell_arg_iso);
  if (map12) { shell_write_vector(map12, shell_arg_map12); 
  igraph_vector_destroy(map12); }
  if (map21) { shell_write_vector(map21, shell_arg_map21); 
  igraph_vector_destroy(map21); }

  return 0;
}

/*-------------------------------------------/
/ igraph_count_subisomorphisms_vf2           /
/-------------------------------------------*/
void shell_igraph_count_subisomorphisms_vf2_usage(char **argv) {
  printf("%s --graph1=<graph1> --graph2=<graph2> --count=<count>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_count_subisomorphisms_vf2(int argc, char **argv) {

  igraph_t graph1;
  igraph_t graph2;
  igraph_integer_t count;
  char* shell_arg_count=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph1",required_argument,0,0 },
                                   { "graph2",required_argument,0,1 },
                                   { "count",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph1 */
      shell_read_graph(&graph1, optarg);
      break;
    case 1: /* graph2 */
      shell_read_graph(&graph2, optarg);
      break;
    case 2: /* count */
      shell_arg_count=strdup(optarg);
      break;
    case 3:
      shell_igraph_count_subisomorphisms_vf2_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_count_subisomorphisms_vf2(&graph1, &graph2, &count);

  /* Write the result */
  igraph_destroy(&graph1);
  igraph_destroy(&graph2);
  shell_write_integer(count, shell_arg_count);

  return 0;
}

/*-------------------------------------------/
/ igraph_get_subisomorphisms_vf2             /
/-------------------------------------------*/
void shell_igraph_get_subisomorphisms_vf2_usage(char **argv) {
  printf("%s --graph1=<graph1> --graph2=<graph2> --maps=<maps>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_get_subisomorphisms_vf2(int argc, char **argv) {

  igraph_t graph1;
  igraph_t graph2;
  igraph_vector_ptr_t maps;
  char* shell_arg_maps=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph1",required_argument,0,0 },
                                   { "graph2",required_argument,0,1 },
                                   { "maps",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph1 */
      shell_read_graph(&graph1, optarg);
      break;
    case 1: /* graph2 */
      shell_read_graph(&graph2, optarg);
      break;
    case 2: /* maps */
      igraph_vector_ptr_init(&maps, 0);
      break;
    case 3:
      shell_igraph_get_subisomorphisms_vf2_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_get_subisomorphisms_vf2(&graph1, &graph2, &maps);

  /* Write the result */
  igraph_destroy(&graph1);
  igraph_destroy(&graph2);
  shell_write_vectorlist(&maps, shell_arg_maps); 
  shell_free_vectorlist(&maps); 
  igraph_vector_ptr_destroy(&maps);

  return 0;
}

/*-------------------------------------------/
/ igraph_isomorphic_34                       /
/-------------------------------------------*/
void shell_igraph_isomorphic_34_usage(char **argv) {
  printf("%s --graph1=<graph1> --graph2=<graph2> --iso=<iso>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_isomorphic_34(int argc, char **argv) {

  igraph_t graph1;
  igraph_t graph2;
  igraph_bool_t iso;
  char* shell_arg_iso=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph1",required_argument,0,0 },
                                   { "graph2",required_argument,0,1 },
                                   { "iso",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph1 */
      shell_read_graph(&graph1, optarg);
      break;
    case 1: /* graph2 */
      shell_read_graph(&graph2, optarg);
      break;
    case 2: /* iso */
      
      break;
    case 3:
      shell_igraph_isomorphic_34_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_isomorphic_34(&graph1, &graph2, &iso);

  /* Write the result */
  igraph_destroy(&graph1);
  igraph_destroy(&graph2);
  shell_write_boolean(iso, shell_arg_iso);

  return 0;
}

/*-------------------------------------------/
/ igraph_canonical_permutation               /
/-------------------------------------------*/
void shell_igraph_canonical_permutation_usage(char **argv) {
  printf("%s --graph=<graph> --labeling=<labeling>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_canonical_permutation(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t labeling;


  char* shell_arg_labeling=0;
  char* shell_arg_info=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "labeling",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* labeling */
      shell_arg_labeling=strdup(optarg); 
  igraph_vector_init(&labeling, 0);
      break;
    case 2:
      shell_igraph_canonical_permutation_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_canonical_permutation(&graph, &labeling, 0, 0);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&labeling, shell_arg_labeling); 
  igraph_vector_destroy(&labeling);

  return 0;
}

/*-------------------------------------------/
/ igraph_permute_vertices                    /
/-------------------------------------------*/
void shell_igraph_permute_vertices_usage(char **argv) {
  printf("%s --graph=<graph> --res=<res> --permutation=<permutation>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_permute_vertices(int argc, char **argv) {

  igraph_t graph;
  igraph_t res;
  igraph_vector_t permutation;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "permutation",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg);
      break;
    case 2: /* permutation */
      shell_read_vector(&permutation, optarg);
      break;
    case 3:
      shell_igraph_permute_vertices_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_permute_vertices(&graph, &res, &permutation);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_graph(&res, shell_arg_res); 
  igraph_destroy(&res);
  igraph_vector_destroy(&permutation);

  return 0;
}

/*-------------------------------------------/
/ igraph_isomorphic_bliss                    /
/-------------------------------------------*/
void shell_igraph_isomorphic_bliss_usage(char **argv) {
  printf("%s --graph1=<graph1> --graph2=<graph2> --iso=<iso> --map12=<map12> --map21=<map21>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_isomorphic_bliss(int argc, char **argv) {

  igraph_t graph1;
  igraph_t graph2;
  igraph_bool_t iso;
  igraph_vector_t v_map12; igraph_vector_t *map12=0;
  igraph_vector_t v_map21; igraph_vector_t *map21=0;




  char* shell_arg_iso=0;
  char* shell_arg_map12=0;
  char* shell_arg_map21=0;
  char* shell_arg_info1=0;
  char* shell_arg_info2=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph1",required_argument,0,0 },
                                   { "graph2",required_argument,0,1 },
                                   { "iso",required_argument,0,2 },
                                   { "map12",required_argument,0,3 },
                                   { "map21",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph1 */
      shell_read_graph(&graph1, optarg);
      break;
    case 1: /* graph2 */
      shell_read_graph(&graph2, optarg);
      break;
    case 2: /* iso */
      
      break;
    case 3: /* map12 */
      map12=&v_map12; igraph_vector_init(map12, 0); 
  shell_arg_map12=strdup(optarg);
      break;
    case 4: /* map21 */
      map21=&v_map21; igraph_vector_init(map21, 0); 
  shell_arg_map21=strdup(optarg);
      break;
    case 5:
      shell_igraph_isomorphic_bliss_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_isomorphic_bliss(&graph1, &graph2, &iso, map12, map21, 0, 0, 0, 0);

  /* Write the result */
  igraph_destroy(&graph1);
  igraph_destroy(&graph2);
  shell_write_boolean(iso, shell_arg_iso);
  if (map12) { shell_write_vector(map12, shell_arg_map12); 
  igraph_vector_destroy(map12); }
  if (map21) { shell_write_vector(map21, shell_arg_map21); 
  igraph_vector_destroy(map21); }

  return 0;
}

/*-------------------------------------------/
/ igraph_automorphisms                       /
/-------------------------------------------*/
void shell_igraph_automorphisms_usage(char **argv) {
  printf("%s --graph=<graph>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_automorphisms(int argc, char **argv) {

  igraph_t graph;


  char* shell_arg_info=0;
  int shell_result;


  int shell_seen[1];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "help",no_argument,0,1 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 1*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1:
      shell_igraph_automorphisms_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<1; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_automorphisms(&graph, 0, 0);

  /* Write the result */
  igraph_destroy(&graph);

  return 0;
}

/*-------------------------------------------/
/ igraph_running_mean                        /
/-------------------------------------------*/
void shell_igraph_running_mean_usage(char **argv) {
  printf("%s --data=<data> --res=<res> --binwidth=<binwidth>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_running_mean(int argc, char **argv) {

  igraph_vector_t data;
  igraph_vector_t res;
  igraph_integer_t binwidth;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "data",required_argument,0,0 },
                                   { "res",required_argument,0,1 },
                                   { "binwidth",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* data */
      shell_read_vector(&data, optarg);
      break;
    case 1: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 2: /* binwidth */
      shell_read_integer(&binwidth, optarg);
      break;
    case 3:
      shell_igraph_running_mean_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_running_mean(&data, &res, binwidth);

  /* Write the result */
  igraph_vector_destroy(&data);
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_random_sample                       /
/-------------------------------------------*/
void shell_igraph_random_sample_usage(char **argv) {
  printf("%s --res=<res> --l=<l> --h=<h> --length=<length>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_random_sample(int argc, char **argv) {

  igraph_vector_t res;
  igraph_integer_t l;
  igraph_integer_t h;
  igraph_integer_t length;
  char* shell_arg_res=0;
  int shell_result;


  int shell_seen[4];
  int shell_index=-1;
  struct option shell_options[]= { { "res",required_argument,0,0 },
                                   { "l",required_argument,0,1 },
                                   { "h",required_argument,0,2 },
                                   { "length",required_argument,0,3 },
                                   { "help",no_argument,0,4 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 4*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* res */
      shell_arg_res=strdup(optarg); 
  igraph_vector_init(&res, 0);
      break;
    case 1: /* l */
      shell_read_integer(&l, optarg);
      break;
    case 2: /* h */
      shell_read_integer(&h, optarg);
      break;
    case 3: /* length */
      shell_read_integer(&length, optarg);
      break;
    case 4:
      shell_igraph_random_sample_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<4; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_random_sample(&res, l, h, length);

  /* Write the result */
  shell_write_vector(&res, shell_arg_res); 
  igraph_vector_destroy(&res);

  return 0;
}

/*-------------------------------------------/
/ igraph_convex_hull                         /
/-------------------------------------------*/
void shell_igraph_convex_hull_usage(char **argv) {
  printf("%s --data=<data> --resverts=<resverts> --rescoords=<rescoords>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_convex_hull(int argc, char **argv) {

  igraph_matrix_t data;
  igraph_vector_t resverts;
  igraph_matrix_t rescoords;
  char* shell_arg_resverts=0;
  char* shell_arg_rescoords=0;
  int shell_result;


  int shell_seen[3];
  int shell_index=-1;
  struct option shell_options[]= { { "data",required_argument,0,0 },
                                   { "resverts",required_argument,0,1 },
                                   { "rescoords",required_argument,0,2 },
                                   { "help",no_argument,0,3 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 3*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* data */
      shell_read_matrix(&data, optarg);
      break;
    case 1: /* resverts */
      shell_arg_resverts=strdup(optarg); 
  igraph_vector_init(&resverts, 0);
      break;
    case 2: /* rescoords */
      shell_arg_rescoords=strdup(optarg); 
  igraph_matrix_init(&rescoords, 0, 0);
      break;
    case 3:
      shell_igraph_convex_hull_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<3; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_convex_hull(&data, &resverts, &rescoords);

  /* Write the result */
  igraph_matrix_destroy(&data);
  shell_write_vector(&resverts, shell_arg_resverts); 
  igraph_vector_destroy(&resverts);
  shell_write_matrix(&rescoords, shell_arg_rescoords); 
  igraph_matrix_destroy(&rescoords);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_d                       /
/-------------------------------------------*/
void shell_igraph_revolver_ml_d_usage(char **argv) {
  printf("%s --graph=<graph> --niter=<niter> --kernel=<kernel> --cites=<cites> --delta=<delta> --filter=<filter> --logprob=<logprob> --logmax=<logmax>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_d(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t niter;
  igraph_vector_t kernel;
  igraph_vector_t v_cites; igraph_vector_t *cites=0;
  igraph_real_t delta=1e-10;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  igraph_real_t logprob;
  igraph_real_t logmax;
  char* shell_arg_kernel=0;
  char* shell_arg_cites=0;
  char* shell_arg_logprob=0;
  char* shell_arg_logmax=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "niter",required_argument,0,1 },
                                   { "kernel",required_argument,0,2 },
                                   { "cites",required_argument,0,3 },
                                   { "delta",required_argument,0,4 },
                                   { "filter",required_argument,0,5 },
                                   { "logprob",required_argument,0,6 },
                                   { "logmax",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 2: /* kernel */
      shell_arg_kernel=strdup(optarg); 
  igraph_vector_init(&kernel, 0);
      break;
    case 3: /* cites */
      cites=&v_cites; igraph_vector_init(cites, 0); 
  shell_arg_cites=strdup(optarg);
      break;
    case 4: /* delta */
      shell_read_real(&delta, optarg);
      break;
    case 5: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 6: /* logprob */
      shell_arg_logprob=strdup(optarg);
      break;
    case 7: /* logmax */
      shell_arg_logmax=strdup(optarg);
      break;
    case 8:
      shell_igraph_revolver_ml_d_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_d(&graph, niter, &kernel, cites, delta, filter, &logprob, &logmax);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&kernel, shell_arg_kernel); 
  igraph_vector_destroy(&kernel);
  if (cites) { shell_write_vector(cites, shell_arg_cites); 
  igraph_vector_destroy(cites); }
  if (filter) { igraph_vector_destroy(filter); }
  shell_write_real(logprob, shell_arg_logprob);
  shell_write_real(logmax, shell_arg_logmax);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_probs_d                    /
/-------------------------------------------*/
void shell_igraph_revolver_probs_d_usage(char **argv) {
  printf("%s --graph=<graph> --kernel=<kernel> --probs=<probs> --citedprobs=<citedprobs> --citingprobs=<citingprobs> --ntk=<ntk>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_probs_d(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t kernel;
  igraph_vector_t v_probs; igraph_vector_t *probs=0;
  igraph_vector_t v_citedprobs; igraph_vector_t *citedprobs=0;
  igraph_vector_t v_citingprobs; igraph_vector_t *citingprobs=0;
  igraph_bool_t ntk=0;
  char* shell_arg_probs=0;
  char* shell_arg_citedprobs=0;
  char* shell_arg_citingprobs=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "kernel",required_argument,0,1 },
                                   { "probs",required_argument,0,2 },
                                   { "citedprobs",required_argument,0,3 },
                                   { "citingprobs",required_argument,0,4 },
                                   { "ntk",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* kernel */
      shell_read_vector(&kernel, optarg);
      break;
    case 2: /* probs */
      probs=&v_probs; igraph_vector_init(probs, 0); 
  shell_arg_probs=strdup(optarg);
      break;
    case 3: /* citedprobs */
      citedprobs=&v_citedprobs; igraph_vector_init(citedprobs, 0); 
  shell_arg_citedprobs=strdup(optarg);
      break;
    case 4: /* citingprobs */
      citingprobs=&v_citingprobs; igraph_vector_init(citingprobs, 0); 
  shell_arg_citingprobs=strdup(optarg);
      break;
    case 5: /* ntk */
      shell_read_boolean(&ntk, optarg);
      break;
    case 6:
      shell_igraph_revolver_probs_d_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_probs_d(&graph, &kernel, probs, citedprobs, citingprobs, ntk);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&kernel);
  if (probs) { shell_write_vector(probs, shell_arg_probs); 
  igraph_vector_destroy(probs); }
  if (citedprobs) { shell_write_vector(citedprobs, shell_arg_citedprobs); 
  igraph_vector_destroy(citedprobs); }
  if (citingprobs) { shell_write_vector(citingprobs, shell_arg_citingprobs); 
  igraph_vector_destroy(citingprobs); }

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_de                      /
/-------------------------------------------*/
void shell_igraph_revolver_ml_de_usage(char **argv) {
  printf("%s --graph=<graph> --niter=<niter> --kernel=<kernel> --cats=<cats> --delta=<delta> --filter=<filter> --logprob=<logprob> --logmax=<logmax>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_de(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t niter;
  igraph_matrix_t kernel;
  igraph_vector_t cats;

  igraph_real_t delta=1e-10;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  igraph_real_t logprob;
  igraph_real_t logmax;
  char* shell_arg_kernel=0;
  char* shell_arg_cites=0;
  char* shell_arg_logprob=0;
  char* shell_arg_logmax=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "niter",required_argument,0,1 },
                                   { "kernel",required_argument,0,2 },
                                   { "cats",required_argument,0,3 },
                                   { "delta",required_argument,0,4 },
                                   { "filter",required_argument,0,5 },
                                   { "logprob",required_argument,0,6 },
                                   { "logmax",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 2: /* kernel */
      shell_arg_kernel=strdup(optarg); 
  igraph_matrix_init(&kernel, 0, 0);
      break;
    case 3: /* cats */
      shell_read_vector(&cats, optarg);
      break;
    case 4: /* delta */
      shell_read_real(&delta, optarg);
      break;
    case 5: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 6: /* logprob */
      shell_arg_logprob=strdup(optarg);
      break;
    case 7: /* logmax */
      shell_arg_logmax=strdup(optarg);
      break;
    case 8:
      shell_igraph_revolver_ml_de_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_de(&graph, niter, &kernel, &cats, 0, delta, filter, &logprob, &logmax);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&kernel, shell_arg_kernel); 
  igraph_matrix_destroy(&kernel);
  igraph_vector_destroy(&cats);
  if (filter) { igraph_vector_destroy(filter); }
  shell_write_real(logprob, shell_arg_logprob);
  shell_write_real(logmax, shell_arg_logmax);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_probs_de                   /
/-------------------------------------------*/
void shell_igraph_revolver_probs_de_usage(char **argv) {
  printf("%s --graph=<graph> --kernel=<kernel> --cats=<cats> --logprobs=<logprobs> --logcited=<logcited> --logciting=<logciting>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_probs_de(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t kernel;
  igraph_vector_t cats;
  igraph_vector_t v_logprobs; igraph_vector_t *logprobs=0;
  igraph_vector_t v_logcited; igraph_vector_t *logcited=0;
  igraph_vector_t v_logciting; igraph_vector_t *logciting=0;
  char* shell_arg_logprobs=0;
  char* shell_arg_logcited=0;
  char* shell_arg_logciting=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "kernel",required_argument,0,1 },
                                   { "cats",required_argument,0,2 },
                                   { "logprobs",required_argument,0,3 },
                                   { "logcited",required_argument,0,4 },
                                   { "logciting",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* kernel */
      shell_read_matrix(&kernel, optarg);
      break;
    case 2: /* cats */
      shell_read_vector(&cats, optarg);
      break;
    case 3: /* logprobs */
      logprobs=&v_logprobs; igraph_vector_init(logprobs, 0); 
  shell_arg_logprobs=strdup(optarg);
      break;
    case 4: /* logcited */
      logcited=&v_logcited; igraph_vector_init(logcited, 0); 
  shell_arg_logcited=strdup(optarg);
      break;
    case 5: /* logciting */
      logciting=&v_logciting; igraph_vector_init(logciting, 0); 
  shell_arg_logciting=strdup(optarg);
      break;
    case 6:
      shell_igraph_revolver_probs_de_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_probs_de(&graph, &kernel, &cats, logprobs, logcited, logciting);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_matrix_destroy(&kernel);
  igraph_vector_destroy(&cats);
  if (logprobs) { shell_write_vector(logprobs, shell_arg_logprobs); 
  igraph_vector_destroy(logprobs); }
  if (logcited) { shell_write_vector(logcited, shell_arg_logcited); 
  igraph_vector_destroy(logcited); }
  if (logciting) { shell_write_vector(logciting, shell_arg_logciting); 
  igraph_vector_destroy(logciting); }

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_ade                     /
/-------------------------------------------*/
void shell_igraph_revolver_ml_ade_usage(char **argv) {
  printf("%s --graph=<graph> --niter=<niter> --cats=<cats> --agebins=<agebins> --delta=<delta> --filter=<filter> --logprob=<logprob> --logmax=<logmax>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_ade(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t niter;

  igraph_vector_t cats;

  igraph_integer_t agebins=300;
  igraph_real_t delta=1e-10;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  igraph_real_t logprob;
  igraph_real_t logmax;
  char* shell_arg_kernel=0;
  char* shell_arg_cites=0;
  char* shell_arg_logprob=0;
  char* shell_arg_logmax=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "niter",required_argument,0,1 },
                                   { "cats",required_argument,0,2 },
                                   { "agebins",required_argument,0,3 },
                                   { "delta",required_argument,0,4 },
                                   { "filter",required_argument,0,5 },
                                   { "logprob",required_argument,0,6 },
                                   { "logmax",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 2: /* cats */
      shell_read_vector(&cats, optarg);
      break;
    case 3: /* agebins */
      shell_read_integer(&agebins, optarg);
      break;
    case 4: /* delta */
      shell_read_real(&delta, optarg);
      break;
    case 5: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 6: /* logprob */
      shell_arg_logprob=strdup(optarg);
      break;
    case 7: /* logmax */
      shell_arg_logmax=strdup(optarg);
      break;
    case 8:
      shell_igraph_revolver_ml_ade_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_ade(&graph, niter, 0, &cats, 0, agebins, delta, filter, &logprob, &logmax);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&cats);
  if (filter) { igraph_vector_destroy(filter); }
  shell_write_real(logprob, shell_arg_logprob);
  shell_write_real(logmax, shell_arg_logmax);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_probs_ade                  /
/-------------------------------------------*/
void shell_igraph_revolver_probs_ade_usage(char **argv) {
  printf("%s --graph=<graph> --cats=<cats> --logprobs=<logprobs> --logcited=<logcited> --logciting=<logciting>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_probs_ade(int argc, char **argv) {

  igraph_t graph;

  igraph_vector_t cats;
  igraph_vector_t v_logprobs; igraph_vector_t *logprobs=0;
  igraph_vector_t v_logcited; igraph_vector_t *logcited=0;
  igraph_vector_t v_logciting; igraph_vector_t *logciting=0;
  char* shell_arg_logprobs=0;
  char* shell_arg_logcited=0;
  char* shell_arg_logciting=0;
  int shell_result;


  int shell_seen[5];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "cats",required_argument,0,1 },
                                   { "logprobs",required_argument,0,2 },
                                   { "logcited",required_argument,0,3 },
                                   { "logciting",required_argument,0,4 },
                                   { "help",no_argument,0,5 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 5*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* cats */
      shell_read_vector(&cats, optarg);
      break;
    case 2: /* logprobs */
      logprobs=&v_logprobs; igraph_vector_init(logprobs, 0); 
  shell_arg_logprobs=strdup(optarg);
      break;
    case 3: /* logcited */
      logcited=&v_logcited; igraph_vector_init(logcited, 0); 
  shell_arg_logcited=strdup(optarg);
      break;
    case 4: /* logciting */
      logciting=&v_logciting; igraph_vector_init(logciting, 0); 
  shell_arg_logciting=strdup(optarg);
      break;
    case 5:
      shell_igraph_revolver_probs_ade_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<5; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_probs_ade(&graph, 0, &cats, logprobs, logcited, logciting);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&cats);
  if (logprobs) { shell_write_vector(logprobs, shell_arg_logprobs); 
  igraph_vector_destroy(logprobs); }
  if (logcited) { shell_write_vector(logcited, shell_arg_logcited); 
  igraph_vector_destroy(logcited); }
  if (logciting) { shell_write_vector(logciting, shell_arg_logciting); 
  igraph_vector_destroy(logciting); }

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_f                       /
/-------------------------------------------*/
void shell_igraph_revolver_ml_f_usage(char **argv) {
  printf("%s --graph=<graph> --niter=<niter> --kernel=<kernel> --cites=<cites> --delta=<delta> --logprob=<logprob> --logmax=<logmax>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_f(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t niter;
  igraph_vector_t kernel;
  igraph_vector_t v_cites; igraph_vector_t *cites=0;
  igraph_real_t delta=1e-10;
  igraph_real_t logprob;
  igraph_real_t logmax;
  char* shell_arg_kernel=0;
  char* shell_arg_cites=0;
  char* shell_arg_logprob=0;
  char* shell_arg_logmax=0;
  int shell_result;


  int shell_seen[7];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "niter",required_argument,0,1 },
                                   { "kernel",required_argument,0,2 },
                                   { "cites",required_argument,0,3 },
                                   { "delta",required_argument,0,4 },
                                   { "logprob",required_argument,0,5 },
                                   { "logmax",required_argument,0,6 },
                                   { "help",no_argument,0,7 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 7*sizeof(int));
  shell_seen[4]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 2: /* kernel */
      shell_arg_kernel=strdup(optarg); 
  igraph_vector_init(&kernel, 0);
      break;
    case 3: /* cites */
      cites=&v_cites; igraph_vector_init(cites, 0); 
  shell_arg_cites=strdup(optarg);
      break;
    case 4: /* delta */
      shell_read_real(&delta, optarg);
      break;
    case 5: /* logprob */
      shell_arg_logprob=strdup(optarg);
      break;
    case 6: /* logmax */
      shell_arg_logmax=strdup(optarg);
      break;
    case 7:
      shell_igraph_revolver_ml_f_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<7; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_f(&graph, niter, &kernel, cites, delta, &logprob, &logmax);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&kernel, shell_arg_kernel); 
  igraph_vector_destroy(&kernel);
  if (cites) { shell_write_vector(cites, shell_arg_cites); 
  igraph_vector_destroy(cites); }
  shell_write_real(logprob, shell_arg_logprob);
  shell_write_real(logmax, shell_arg_logmax);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_df                      /
/-------------------------------------------*/
void shell_igraph_revolver_ml_df_usage(char **argv) {
  printf("%s --graph=<graph> --niter=<niter> --kernel=<kernel> --delta=<delta> --logprob=<logprob> --logmax=<logmax>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_df(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t niter;
  igraph_matrix_t kernel;

  igraph_real_t delta=1e-10;
  igraph_real_t logprob;
  igraph_real_t logmax;
  char* shell_arg_kernel=0;
  char* shell_arg_cites=0;
  char* shell_arg_logprob=0;
  char* shell_arg_logmax=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "niter",required_argument,0,1 },
                                   { "kernel",required_argument,0,2 },
                                   { "delta",required_argument,0,3 },
                                   { "logprob",required_argument,0,4 },
                                   { "logmax",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[3]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 2: /* kernel */
      shell_arg_kernel=strdup(optarg); 
  igraph_matrix_init(&kernel, 0, 0);
      break;
    case 3: /* delta */
      shell_read_real(&delta, optarg);
      break;
    case 4: /* logprob */
      shell_arg_logprob=strdup(optarg);
      break;
    case 5: /* logmax */
      shell_arg_logmax=strdup(optarg);
      break;
    case 6:
      shell_igraph_revolver_ml_df_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_df(&graph, niter, &kernel, 0, delta, &logprob, &logmax);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&kernel, shell_arg_kernel); 
  igraph_matrix_destroy(&kernel);
  shell_write_real(logprob, shell_arg_logprob);
  shell_write_real(logmax, shell_arg_logmax);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_l                       /
/-------------------------------------------*/
void shell_igraph_revolver_ml_l_usage(char **argv) {
  printf("%s --graph=<graph> --niter=<niter> --kernel=<kernel> --cites=<cites> --agebins=<agebins> --delta=<delta> --logprob=<logprob> --logmax=<logmax>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_l(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t niter;
  igraph_vector_t kernel;
  igraph_vector_t v_cites; igraph_vector_t *cites=0;
  igraph_integer_t agebins=300;
  igraph_real_t delta=1e-10;
  igraph_real_t logprob;
  igraph_real_t logmax;
  char* shell_arg_kernel=0;
  char* shell_arg_cites=0;
  char* shell_arg_logprob=0;
  char* shell_arg_logmax=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "niter",required_argument,0,1 },
                                   { "kernel",required_argument,0,2 },
                                   { "cites",required_argument,0,3 },
                                   { "agebins",required_argument,0,4 },
                                   { "delta",required_argument,0,5 },
                                   { "logprob",required_argument,0,6 },
                                   { "logmax",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 2: /* kernel */
      shell_arg_kernel=strdup(optarg); 
  igraph_vector_init(&kernel, 0);
      break;
    case 3: /* cites */
      cites=&v_cites; igraph_vector_init(cites, 0); 
  shell_arg_cites=strdup(optarg);
      break;
    case 4: /* agebins */
      shell_read_integer(&agebins, optarg);
      break;
    case 5: /* delta */
      shell_read_real(&delta, optarg);
      break;
    case 6: /* logprob */
      shell_arg_logprob=strdup(optarg);
      break;
    case 7: /* logmax */
      shell_arg_logmax=strdup(optarg);
      break;
    case 8:
      shell_igraph_revolver_ml_l_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_l(&graph, niter, &kernel, cites, agebins, delta, &logprob, &logmax);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&kernel, shell_arg_kernel); 
  igraph_vector_destroy(&kernel);
  if (cites) { shell_write_vector(cites, shell_arg_cites); 
  igraph_vector_destroy(cites); }
  shell_write_real(logprob, shell_arg_logprob);
  shell_write_real(logmax, shell_arg_logmax);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_ad                      /
/-------------------------------------------*/
void shell_igraph_revolver_ml_ad_usage(char **argv) {
  printf("%s --graph=<graph> --niter=<niter> --kernel=<kernel> --agebins=<agebins> --delta=<delta> --filter=<filter> --logprob=<logprob> --logmax=<logmax>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_ad(int argc, char **argv) {

  igraph_t graph;
  igraph_integer_t niter;
  igraph_matrix_t kernel;

  igraph_integer_t agebins=300;
  igraph_real_t delta=1e-10;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  igraph_real_t logprob;
  igraph_real_t logmax;
  char* shell_arg_kernel=0;
  char* shell_arg_cites=0;
  char* shell_arg_logprob=0;
  char* shell_arg_logmax=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "niter",required_argument,0,1 },
                                   { "kernel",required_argument,0,2 },
                                   { "agebins",required_argument,0,3 },
                                   { "delta",required_argument,0,4 },
                                   { "filter",required_argument,0,5 },
                                   { "logprob",required_argument,0,6 },
                                   { "logmax",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));
  shell_seen[3]=2;
  shell_seen[4]=2;
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* niter */
      shell_read_integer(&niter, optarg);
      break;
    case 2: /* kernel */
      shell_arg_kernel=strdup(optarg); 
  igraph_matrix_init(&kernel, 0, 0);
      break;
    case 3: /* agebins */
      shell_read_integer(&agebins, optarg);
      break;
    case 4: /* delta */
      shell_read_real(&delta, optarg);
      break;
    case 5: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 6: /* logprob */
      shell_arg_logprob=strdup(optarg);
      break;
    case 7: /* logmax */
      shell_arg_logmax=strdup(optarg);
      break;
    case 8:
      shell_igraph_revolver_ml_ad_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_ad(&graph, niter, &kernel, 0, agebins, delta, filter, &logprob, &logmax);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_matrix(&kernel, shell_arg_kernel); 
  igraph_matrix_destroy(&kernel);
  if (filter) { igraph_vector_destroy(filter); }
  shell_write_real(logprob, shell_arg_logprob);
  shell_write_real(logmax, shell_arg_logmax);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_probs_ad                   /
/-------------------------------------------*/
void shell_igraph_revolver_probs_ad_usage(char **argv) {
  printf("%s --graph=<graph> --kernel=<kernel> --probs=<probs> --citedprobs=<citedprobs> --citingprobs=<citingprobs> --ntk=<ntk>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_probs_ad(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t kernel;
  igraph_vector_t v_probs; igraph_vector_t *probs=0;
  igraph_vector_t v_citedprobs; igraph_vector_t *citedprobs=0;
  igraph_vector_t v_citingprobs; igraph_vector_t *citingprobs=0;
  igraph_bool_t ntk=0;
  char* shell_arg_probs=0;
  char* shell_arg_citedprobs=0;
  char* shell_arg_citingprobs=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "kernel",required_argument,0,1 },
                                   { "probs",required_argument,0,2 },
                                   { "citedprobs",required_argument,0,3 },
                                   { "citingprobs",required_argument,0,4 },
                                   { "ntk",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* kernel */
      shell_read_matrix(&kernel, optarg);
      break;
    case 2: /* probs */
      probs=&v_probs; igraph_vector_init(probs, 0); 
  shell_arg_probs=strdup(optarg);
      break;
    case 3: /* citedprobs */
      citedprobs=&v_citedprobs; igraph_vector_init(citedprobs, 0); 
  shell_arg_citedprobs=strdup(optarg);
      break;
    case 4: /* citingprobs */
      citingprobs=&v_citingprobs; igraph_vector_init(citingprobs, 0); 
  shell_arg_citingprobs=strdup(optarg);
      break;
    case 5: /* ntk */
      shell_read_boolean(&ntk, optarg);
      break;
    case 6:
      shell_igraph_revolver_probs_ad_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_probs_ad(&graph, &kernel, probs, citedprobs, citingprobs, ntk);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_matrix_destroy(&kernel);
  if (probs) { shell_write_vector(probs, shell_arg_probs); 
  igraph_vector_destroy(probs); }
  if (citedprobs) { shell_write_vector(citedprobs, shell_arg_citedprobs); 
  igraph_vector_destroy(citedprobs); }
  if (citingprobs) { shell_write_vector(citingprobs, shell_arg_citingprobs); 
  igraph_vector_destroy(citingprobs); }

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_D_alpha                 /
/-------------------------------------------*/
void shell_igraph_revolver_ml_D_alpha_usage(char **argv) {
  printf("%s --graph=<graph> --alpha=<alpha> --alpha-out=<alpha-out> --Fmin=<Fmin> --abstol=<abstol> --reltol=<reltol> --maxit=<maxit> --filter=<filter> --fncount=<fncount> --grcount=<grcount>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_D_alpha(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t alpha;
  igraph_real_t Fmin;
  igraph_real_t abstol=1e-8;
  igraph_real_t reltol=1e-8;
  int maxit=1000;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  igraph_integer_t fncount;
  igraph_integer_t grcount;
  char* shell_arg_alpha=0;
  char* shell_arg_Fmin=0;
  char* shell_arg_fncount=0;
  char* shell_arg_grcount=0;
  int shell_result;


  int shell_seen[10];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "alpha",required_argument,0,1 },
                                   { "alpha-out",required_argument,0,2 },
                                   { "Fmin",required_argument,0,3 },
                                   { "abstol",required_argument,0,4 },
                                   { "reltol",required_argument,0,5 },
                                   { "maxit",required_argument,0,6 },
                                   { "filter",required_argument,0,7 },
                                   { "fncount",required_argument,0,8 },
                                   { "grcount",required_argument,0,9 },
                                   { "help",no_argument,0,10 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 10*sizeof(int));
  shell_seen[4]=2;
  shell_seen[5]=2;
  shell_seen[6]=2;
  shell_seen[7]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* alpha */
      shell_read_real(&alpha, optarg);
      break;
    case 2: /* alpha-out */
      shell_arg_alpha=strdup(optarg);
      break;
    case 3: /* Fmin */
      shell_arg_Fmin=strdup(optarg);
      break;
    case 4: /* abstol */
      shell_read_real(&abstol, optarg);
      break;
    case 5: /* reltol */
      shell_read_real(&reltol, optarg);
      break;
    case 6: /* maxit */
      shell_read_int(&maxit, optarg);
      break;
    case 7: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 8: /* fncount */
      shell_arg_fncount=strdup(optarg);
      break;
    case 9: /* grcount */
      shell_arg_grcount=strdup(optarg);
      break;
    case 10:
      shell_igraph_revolver_ml_D_alpha_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<10; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_D_alpha(&graph, &alpha, &Fmin, abstol, reltol, maxit, filter, &fncount, &grcount);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(alpha, shell_arg_alpha);
  shell_write_real(Fmin, shell_arg_Fmin);
  if (filter) { igraph_vector_destroy(filter); }
  shell_write_integer(fncount, shell_arg_fncount);
  shell_write_integer(grcount, shell_arg_grcount);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_D_alpha_a               /
/-------------------------------------------*/
void shell_igraph_revolver_ml_D_alpha_a_usage(char **argv) {
  printf("%s --graph=<graph> --alpha=<alpha> --alpha-out=<alpha-out> --a=<a> --a-out=<a-out> --Fmin=<Fmin> --abstol=<abstol> --reltol=<reltol> --maxit=<maxit> --filter=<filter> --fncount=<fncount> --grcount=<grcount>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_D_alpha_a(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t alpha;
  igraph_real_t a;
  igraph_real_t Fmin;
  igraph_real_t abstol=1e-8;
  igraph_real_t reltol=1e-8;
  int maxit=1000;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  igraph_integer_t fncount;
  igraph_integer_t grcount;
  char* shell_arg_alpha=0;
  char* shell_arg_a=0;
  char* shell_arg_Fmin=0;
  char* shell_arg_fncount=0;
  char* shell_arg_grcount=0;
  int shell_result;


  int shell_seen[12];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "alpha",required_argument,0,1 },
                                   { "alpha-out",required_argument,0,2 },
                                   { "a",required_argument,0,3 },
                                   { "a-out",required_argument,0,4 },
                                   { "Fmin",required_argument,0,5 },
                                   { "abstol",required_argument,0,6 },
                                   { "reltol",required_argument,0,7 },
                                   { "maxit",required_argument,0,8 },
                                   { "filter",required_argument,0,9 },
                                   { "fncount",required_argument,0,10 },
                                   { "grcount",required_argument,0,11 },
                                   { "help",no_argument,0,12 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 12*sizeof(int));
  shell_seen[6]=2;
  shell_seen[7]=2;
  shell_seen[8]=2;
  shell_seen[9]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* alpha */
      shell_read_real(&alpha, optarg);
      break;
    case 2: /* alpha-out */
      shell_arg_alpha=strdup(optarg);
      break;
    case 3: /* a */
      shell_read_real(&a, optarg);
      break;
    case 4: /* a-out */
      shell_arg_a=strdup(optarg);
      break;
    case 5: /* Fmin */
      shell_arg_Fmin=strdup(optarg);
      break;
    case 6: /* abstol */
      shell_read_real(&abstol, optarg);
      break;
    case 7: /* reltol */
      shell_read_real(&reltol, optarg);
      break;
    case 8: /* maxit */
      shell_read_int(&maxit, optarg);
      break;
    case 9: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 10: /* fncount */
      shell_arg_fncount=strdup(optarg);
      break;
    case 11: /* grcount */
      shell_arg_grcount=strdup(optarg);
      break;
    case 12:
      shell_igraph_revolver_ml_D_alpha_a_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<12; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_D_alpha_a(&graph, &alpha, &a, &Fmin, abstol, reltol, maxit, filter, &fncount, &grcount);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(alpha, shell_arg_alpha);
  shell_write_real(a, shell_arg_a);
  shell_write_real(Fmin, shell_arg_Fmin);
  if (filter) { igraph_vector_destroy(filter); }
  shell_write_integer(fncount, shell_arg_fncount);
  shell_write_integer(grcount, shell_arg_grcount);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_DE_alpha_a              /
/-------------------------------------------*/
void shell_igraph_revolver_ml_DE_alpha_a_usage(char **argv) {
  printf("%s --graph=<graph> --cats=<cats> --alpha=<alpha> --alpha-out=<alpha-out> --a=<a> --a-out=<a-out> --coeffs=<coeffs> --coeffs-out=<coeffs-out> --Fmin=<Fmin> --abstol=<abstol> --reltol=<reltol> --maxit=<maxit> --filter=<filter> --fncount=<fncount> --grcount=<grcount>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_DE_alpha_a(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t cats;
  igraph_real_t alpha;
  igraph_real_t a;
  igraph_vector_t coeffs;
  igraph_real_t Fmin;
  igraph_real_t abstol=1e-8;
  igraph_real_t reltol=1e-8;
  int maxit=1000;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  igraph_integer_t fncount;
  igraph_integer_t grcount;
  char* shell_arg_alpha=0;
  char* shell_arg_a=0;
  char* shell_arg_coeffs=0;
  char* shell_arg_Fmin=0;
  char* shell_arg_fncount=0;
  char* shell_arg_grcount=0;
  int shell_result;


  int shell_seen[15];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "cats",required_argument,0,1 },
                                   { "alpha",required_argument,0,2 },
                                   { "alpha-out",required_argument,0,3 },
                                   { "a",required_argument,0,4 },
                                   { "a-out",required_argument,0,5 },
                                   { "coeffs",required_argument,0,6 },
                                   { "coeffs-out",required_argument,0,7 },
                                   { "Fmin",required_argument,0,8 },
                                   { "abstol",required_argument,0,9 },
                                   { "reltol",required_argument,0,10 },
                                   { "maxit",required_argument,0,11 },
                                   { "filter",required_argument,0,12 },
                                   { "fncount",required_argument,0,13 },
                                   { "grcount",required_argument,0,14 },
                                   { "help",no_argument,0,15 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 15*sizeof(int));
  shell_seen[9]=2;
  shell_seen[10]=2;
  shell_seen[11]=2;
  shell_seen[12]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* cats */
      shell_read_vector(&cats, optarg);
      break;
    case 2: /* alpha */
      shell_read_real(&alpha, optarg);
      break;
    case 3: /* alpha-out */
      shell_arg_alpha=strdup(optarg);
      break;
    case 4: /* a */
      shell_read_real(&a, optarg);
      break;
    case 5: /* a-out */
      shell_arg_a=strdup(optarg);
      break;
    case 6: /* coeffs */
      shell_read_vector(&coeffs, optarg);
      break;
    case 7: /* coeffs-out */
      shell_arg_coeffs=strdup(optarg); 
  igraph_vector_init(&coeffs, 0);
      break;
    case 8: /* Fmin */
      shell_arg_Fmin=strdup(optarg);
      break;
    case 9: /* abstol */
      shell_read_real(&abstol, optarg);
      break;
    case 10: /* reltol */
      shell_read_real(&reltol, optarg);
      break;
    case 11: /* maxit */
      shell_read_int(&maxit, optarg);
      break;
    case 12: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 13: /* fncount */
      shell_arg_fncount=strdup(optarg);
      break;
    case 14: /* grcount */
      shell_arg_grcount=strdup(optarg);
      break;
    case 15:
      shell_igraph_revolver_ml_DE_alpha_a_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<15; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_DE_alpha_a(&graph, &cats, &alpha, &a, &coeffs, &Fmin, abstol, reltol, maxit, filter, &fncount, &grcount);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&cats);
  shell_write_real(alpha, shell_arg_alpha);
  shell_write_real(a, shell_arg_a);
  igraph_vector_destroy(&coeffs);
  shell_write_vector(&coeffs, shell_arg_coeffs); 
  igraph_vector_destroy(&coeffs);
  shell_write_real(Fmin, shell_arg_Fmin);
  if (filter) { igraph_vector_destroy(filter); }
  shell_write_integer(fncount, shell_arg_fncount);
  shell_write_integer(grcount, shell_arg_grcount);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_AD_alpha_a_beta         /
/-------------------------------------------*/
void shell_igraph_revolver_ml_AD_alpha_a_beta_usage(char **argv) {
  printf("%s --graph=<graph> --alpha=<alpha> --alpha-out=<alpha-out> --a=<a> --a-out=<a-out> --beta=<beta> --beta-out=<beta-out> --Fmin=<Fmin> --abstol=<abstol> --reltol=<reltol> --maxit=<maxit> --agebins=<agebins> --filter=<filter> --fncount=<fncount> --grcount=<grcount>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_AD_alpha_a_beta(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t alpha;
  igraph_real_t a;
  igraph_real_t beta;
  igraph_real_t Fmin;
  igraph_real_t abstol=1e-8;
  igraph_real_t reltol=1e-8;
  int maxit=1000;
  int agebins=300;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  igraph_integer_t fncount;
  igraph_integer_t grcount;
  char* shell_arg_alpha=0;
  char* shell_arg_a=0;
  char* shell_arg_beta=0;
  char* shell_arg_Fmin=0;
  char* shell_arg_fncount=0;
  char* shell_arg_grcount=0;
  int shell_result;


  int shell_seen[15];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "alpha",required_argument,0,1 },
                                   { "alpha-out",required_argument,0,2 },
                                   { "a",required_argument,0,3 },
                                   { "a-out",required_argument,0,4 },
                                   { "beta",required_argument,0,5 },
                                   { "beta-out",required_argument,0,6 },
                                   { "Fmin",required_argument,0,7 },
                                   { "abstol",required_argument,0,8 },
                                   { "reltol",required_argument,0,9 },
                                   { "maxit",required_argument,0,10 },
                                   { "agebins",required_argument,0,11 },
                                   { "filter",required_argument,0,12 },
                                   { "fncount",required_argument,0,13 },
                                   { "grcount",required_argument,0,14 },
                                   { "help",no_argument,0,15 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 15*sizeof(int));
  shell_seen[8]=2;
  shell_seen[9]=2;
  shell_seen[10]=2;
  shell_seen[11]=2;
  shell_seen[12]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* alpha */
      shell_read_real(&alpha, optarg);
      break;
    case 2: /* alpha-out */
      shell_arg_alpha=strdup(optarg);
      break;
    case 3: /* a */
      shell_read_real(&a, optarg);
      break;
    case 4: /* a-out */
      shell_arg_a=strdup(optarg);
      break;
    case 5: /* beta */
      shell_read_real(&beta, optarg);
      break;
    case 6: /* beta-out */
      shell_arg_beta=strdup(optarg);
      break;
    case 7: /* Fmin */
      shell_arg_Fmin=strdup(optarg);
      break;
    case 8: /* abstol */
      shell_read_real(&abstol, optarg);
      break;
    case 9: /* reltol */
      shell_read_real(&reltol, optarg);
      break;
    case 10: /* maxit */
      shell_read_int(&maxit, optarg);
      break;
    case 11: /* agebins */
      shell_read_int(&agebins, optarg);
      break;
    case 12: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 13: /* fncount */
      shell_arg_fncount=strdup(optarg);
      break;
    case 14: /* grcount */
      shell_arg_grcount=strdup(optarg);
      break;
    case 15:
      shell_igraph_revolver_ml_AD_alpha_a_beta_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<15; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_AD_alpha_a_beta(&graph, &alpha, &a, &beta, &Fmin, abstol, reltol, maxit, agebins, filter, &fncount, &grcount);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(alpha, shell_arg_alpha);
  shell_write_real(a, shell_arg_a);
  shell_write_real(beta, shell_arg_beta);
  shell_write_real(Fmin, shell_arg_Fmin);
  if (filter) { igraph_vector_destroy(filter); }
  shell_write_integer(fncount, shell_arg_fncount);
  shell_write_integer(grcount, shell_arg_grcount);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_AD_dpareto              /
/-------------------------------------------*/
void shell_igraph_revolver_ml_AD_dpareto_usage(char **argv) {
  printf("%s --graph=<graph> --alpha=<alpha> --alpha-out=<alpha-out> --a=<a> --a-out=<a-out> --paralpha=<paralpha> --paralpha-out=<paralpha-out> --parbeta=<parbeta> --parbeta-out=<parbeta-out> --parscale=<parscale> --parscale-out=<parscale-out> --Fmin=<Fmin> --abstol=<abstol> --reltol=<reltol> --maxit=<maxit> --agebins=<agebins> --filter=<filter> --fncount=<fncount> --grcount=<grcount>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_AD_dpareto(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t alpha;
  igraph_real_t a;
  igraph_real_t paralpha;
  igraph_real_t parbeta;
  igraph_real_t parscale;
  igraph_real_t Fmin;
  igraph_real_t abstol=1e-8;
  igraph_real_t reltol=1e-8;
  int maxit=1000;
  int agebins=300;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  igraph_integer_t fncount;
  igraph_integer_t grcount;
  char* shell_arg_alpha=0;
  char* shell_arg_a=0;
  char* shell_arg_paralpha=0;
  char* shell_arg_parbeta=0;
  char* shell_arg_parscale=0;
  char* shell_arg_Fmin=0;
  char* shell_arg_fncount=0;
  char* shell_arg_grcount=0;
  int shell_result;


  int shell_seen[19];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "alpha",required_argument,0,1 },
                                   { "alpha-out",required_argument,0,2 },
                                   { "a",required_argument,0,3 },
                                   { "a-out",required_argument,0,4 },
                                   { "paralpha",required_argument,0,5 },
                                   { "paralpha-out",required_argument,0,6 },
                                   { "parbeta",required_argument,0,7 },
                                   { "parbeta-out",required_argument,0,8 },
                                   { "parscale",required_argument,0,9 },
                                   { "parscale-out",required_argument,0,10 },
                                   { "Fmin",required_argument,0,11 },
                                   { "abstol",required_argument,0,12 },
                                   { "reltol",required_argument,0,13 },
                                   { "maxit",required_argument,0,14 },
                                   { "agebins",required_argument,0,15 },
                                   { "filter",required_argument,0,16 },
                                   { "fncount",required_argument,0,17 },
                                   { "grcount",required_argument,0,18 },
                                   { "help",no_argument,0,19 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 19*sizeof(int));
  shell_seen[12]=2;
  shell_seen[13]=2;
  shell_seen[14]=2;
  shell_seen[15]=2;
  shell_seen[16]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* alpha */
      shell_read_real(&alpha, optarg);
      break;
    case 2: /* alpha-out */
      shell_arg_alpha=strdup(optarg);
      break;
    case 3: /* a */
      shell_read_real(&a, optarg);
      break;
    case 4: /* a-out */
      shell_arg_a=strdup(optarg);
      break;
    case 5: /* paralpha */
      shell_read_real(&paralpha, optarg);
      break;
    case 6: /* paralpha-out */
      shell_arg_paralpha=strdup(optarg);
      break;
    case 7: /* parbeta */
      shell_read_real(&parbeta, optarg);
      break;
    case 8: /* parbeta-out */
      shell_arg_parbeta=strdup(optarg);
      break;
    case 9: /* parscale */
      shell_read_real(&parscale, optarg);
      break;
    case 10: /* parscale-out */
      shell_arg_parscale=strdup(optarg);
      break;
    case 11: /* Fmin */
      shell_arg_Fmin=strdup(optarg);
      break;
    case 12: /* abstol */
      shell_read_real(&abstol, optarg);
      break;
    case 13: /* reltol */
      shell_read_real(&reltol, optarg);
      break;
    case 14: /* maxit */
      shell_read_int(&maxit, optarg);
      break;
    case 15: /* agebins */
      shell_read_int(&agebins, optarg);
      break;
    case 16: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 17: /* fncount */
      shell_arg_fncount=strdup(optarg);
      break;
    case 18: /* grcount */
      shell_arg_grcount=strdup(optarg);
      break;
    case 19:
      shell_igraph_revolver_ml_AD_dpareto_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<19; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_AD_dpareto(&graph, &alpha, &a, &paralpha, &parbeta, &parscale, &Fmin, abstol, reltol, maxit, agebins, filter, &fncount, &grcount);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(alpha, shell_arg_alpha);
  shell_write_real(a, shell_arg_a);
  shell_write_real(paralpha, shell_arg_paralpha);
  shell_write_real(parbeta, shell_arg_parbeta);
  shell_write_real(parscale, shell_arg_parscale);
  shell_write_real(Fmin, shell_arg_Fmin);
  if (filter) { igraph_vector_destroy(filter); }
  shell_write_integer(fncount, shell_arg_fncount);
  shell_write_integer(grcount, shell_arg_grcount);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_AD_dpareto_eval         /
/-------------------------------------------*/
void shell_igraph_revolver_ml_AD_dpareto_eval_usage(char **argv) {
  printf("%s --graph=<graph> --alpha=<alpha> --a=<a> --paralpha=<paralpha> --parbeta=<parbeta> --parscale=<parscale> --value=<value> --deriv=<deriv> --agebins=<agebins> --filter=<filter>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_AD_dpareto_eval(int argc, char **argv) {

  igraph_t graph;
  igraph_real_t alpha;
  igraph_real_t a;
  igraph_real_t paralpha;
  igraph_real_t parbeta;
  igraph_real_t parscale;
  igraph_real_t value;
  igraph_vector_t deriv;
  int agebins=300;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  char* shell_arg_value=0;
  char* shell_arg_deriv=0;
  int shell_result;


  int shell_seen[10];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "alpha",required_argument,0,1 },
                                   { "a",required_argument,0,2 },
                                   { "paralpha",required_argument,0,3 },
                                   { "parbeta",required_argument,0,4 },
                                   { "parscale",required_argument,0,5 },
                                   { "value",required_argument,0,6 },
                                   { "deriv",required_argument,0,7 },
                                   { "agebins",required_argument,0,8 },
                                   { "filter",required_argument,0,9 },
                                   { "help",no_argument,0,10 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 10*sizeof(int));
  shell_seen[8]=2;
  shell_seen[9]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* alpha */
      shell_read_real(&alpha, optarg);
      break;
    case 2: /* a */
      shell_read_real(&a, optarg);
      break;
    case 3: /* paralpha */
      shell_read_real(&paralpha, optarg);
      break;
    case 4: /* parbeta */
      shell_read_real(&parbeta, optarg);
      break;
    case 5: /* parscale */
      shell_read_real(&parscale, optarg);
      break;
    case 6: /* value */
      shell_arg_value=strdup(optarg);
      break;
    case 7: /* deriv */
      shell_arg_deriv=strdup(optarg); 
  igraph_vector_init(&deriv, 0);
      break;
    case 8: /* agebins */
      shell_read_int(&agebins, optarg);
      break;
    case 9: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 10:
      shell_igraph_revolver_ml_AD_dpareto_eval_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<10; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_AD_dpareto_eval(&graph, alpha, a, paralpha, parbeta, parscale, &value, &deriv, agebins, filter);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_real(value, shell_arg_value);
  shell_write_vector(&deriv, shell_arg_deriv); 
  igraph_vector_destroy(&deriv);
  if (filter) { igraph_vector_destroy(filter); }

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_ADE_alpha_a_beta        /
/-------------------------------------------*/
void shell_igraph_revolver_ml_ADE_alpha_a_beta_usage(char **argv) {
  printf("%s --graph=<graph> --cats=<cats> --alpha=<alpha> --alpha-out=<alpha-out> --a=<a> --a-out=<a-out> --beta=<beta> --beta-out=<beta-out> --coeffs=<coeffs> --coeffs-out=<coeffs-out> --Fmin=<Fmin> --abstol=<abstol> --reltol=<reltol> --maxit=<maxit> --agebins=<agebins> --filter=<filter> --fncount=<fncount> --grcount=<grcount>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_ADE_alpha_a_beta(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t cats;
  igraph_real_t alpha;
  igraph_real_t a;
  igraph_real_t beta;
  igraph_vector_t coeffs;
  igraph_real_t Fmin;
  igraph_real_t abstol=1e-8;
  igraph_real_t reltol=1e-8;
  int maxit=1000;
  int agebins=300;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  igraph_integer_t fncount;
  igraph_integer_t grcount;
  char* shell_arg_alpha=0;
  char* shell_arg_a=0;
  char* shell_arg_beta=0;
  char* shell_arg_coeffs=0;
  char* shell_arg_Fmin=0;
  char* shell_arg_fncount=0;
  char* shell_arg_grcount=0;
  int shell_result;


  int shell_seen[18];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "cats",required_argument,0,1 },
                                   { "alpha",required_argument,0,2 },
                                   { "alpha-out",required_argument,0,3 },
                                   { "a",required_argument,0,4 },
                                   { "a-out",required_argument,0,5 },
                                   { "beta",required_argument,0,6 },
                                   { "beta-out",required_argument,0,7 },
                                   { "coeffs",required_argument,0,8 },
                                   { "coeffs-out",required_argument,0,9 },
                                   { "Fmin",required_argument,0,10 },
                                   { "abstol",required_argument,0,11 },
                                   { "reltol",required_argument,0,12 },
                                   { "maxit",required_argument,0,13 },
                                   { "agebins",required_argument,0,14 },
                                   { "filter",required_argument,0,15 },
                                   { "fncount",required_argument,0,16 },
                                   { "grcount",required_argument,0,17 },
                                   { "help",no_argument,0,18 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 18*sizeof(int));
  shell_seen[11]=2;
  shell_seen[12]=2;
  shell_seen[13]=2;
  shell_seen[14]=2;
  shell_seen[15]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* cats */
      shell_read_vector(&cats, optarg);
      break;
    case 2: /* alpha */
      shell_read_real(&alpha, optarg);
      break;
    case 3: /* alpha-out */
      shell_arg_alpha=strdup(optarg);
      break;
    case 4: /* a */
      shell_read_real(&a, optarg);
      break;
    case 5: /* a-out */
      shell_arg_a=strdup(optarg);
      break;
    case 6: /* beta */
      shell_read_real(&beta, optarg);
      break;
    case 7: /* beta-out */
      shell_arg_beta=strdup(optarg);
      break;
    case 8: /* coeffs */
      shell_read_vector(&coeffs, optarg);
      break;
    case 9: /* coeffs-out */
      shell_arg_coeffs=strdup(optarg); 
  igraph_vector_init(&coeffs, 0);
      break;
    case 10: /* Fmin */
      shell_arg_Fmin=strdup(optarg);
      break;
    case 11: /* abstol */
      shell_read_real(&abstol, optarg);
      break;
    case 12: /* reltol */
      shell_read_real(&reltol, optarg);
      break;
    case 13: /* maxit */
      shell_read_int(&maxit, optarg);
      break;
    case 14: /* agebins */
      shell_read_int(&agebins, optarg);
      break;
    case 15: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 16: /* fncount */
      shell_arg_fncount=strdup(optarg);
      break;
    case 17: /* grcount */
      shell_arg_grcount=strdup(optarg);
      break;
    case 18:
      shell_igraph_revolver_ml_ADE_alpha_a_beta_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<18; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_ADE_alpha_a_beta(&graph, &cats, &alpha, &a, &beta, &coeffs, &Fmin, abstol, reltol, maxit, agebins, filter, &fncount, &grcount);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&cats);
  shell_write_real(alpha, shell_arg_alpha);
  shell_write_real(a, shell_arg_a);
  shell_write_real(beta, shell_arg_beta);
  igraph_vector_destroy(&coeffs);
  shell_write_vector(&coeffs, shell_arg_coeffs); 
  igraph_vector_destroy(&coeffs);
  shell_write_real(Fmin, shell_arg_Fmin);
  if (filter) { igraph_vector_destroy(filter); }
  shell_write_integer(fncount, shell_arg_fncount);
  shell_write_integer(grcount, shell_arg_grcount);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_ADE_dpareto             /
/-------------------------------------------*/
void shell_igraph_revolver_ml_ADE_dpareto_usage(char **argv) {
  printf("%s --graph=<graph> --cats=<cats> --alpha=<alpha> --alpha-out=<alpha-out> --a=<a> --a-out=<a-out> --paralpha=<paralpha> --paralpha-out=<paralpha-out> --parbeta=<parbeta> --parbeta-out=<parbeta-out> --parscale=<parscale> --parscale-out=<parscale-out> --coeffs=<coeffs> --coeffs-out=<coeffs-out> --Fmin=<Fmin> --abstol=<abstol> --reltol=<reltol> --maxit=<maxit> --agebins=<agebins> --filter=<filter> --fncount=<fncount> --grcount=<grcount>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_ADE_dpareto(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t cats;
  igraph_real_t alpha;
  igraph_real_t a;
  igraph_real_t paralpha;
  igraph_real_t parbeta;
  igraph_real_t parscale;
  igraph_vector_t coeffs;
  igraph_real_t Fmin;
  igraph_real_t abstol=1e-8;
  igraph_real_t reltol=1e-8;
  int maxit=1000;
  int agebins=300;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  igraph_integer_t fncount;
  igraph_integer_t grcount;
  char* shell_arg_alpha=0;
  char* shell_arg_a=0;
  char* shell_arg_paralpha=0;
  char* shell_arg_parbeta=0;
  char* shell_arg_parscale=0;
  char* shell_arg_coeffs=0;
  char* shell_arg_Fmin=0;
  char* shell_arg_fncount=0;
  char* shell_arg_grcount=0;
  int shell_result;


  int shell_seen[22];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "cats",required_argument,0,1 },
                                   { "alpha",required_argument,0,2 },
                                   { "alpha-out",required_argument,0,3 },
                                   { "a",required_argument,0,4 },
                                   { "a-out",required_argument,0,5 },
                                   { "paralpha",required_argument,0,6 },
                                   { "paralpha-out",required_argument,0,7 },
                                   { "parbeta",required_argument,0,8 },
                                   { "parbeta-out",required_argument,0,9 },
                                   { "parscale",required_argument,0,10 },
                                   { "parscale-out",required_argument,0,11 },
                                   { "coeffs",required_argument,0,12 },
                                   { "coeffs-out",required_argument,0,13 },
                                   { "Fmin",required_argument,0,14 },
                                   { "abstol",required_argument,0,15 },
                                   { "reltol",required_argument,0,16 },
                                   { "maxit",required_argument,0,17 },
                                   { "agebins",required_argument,0,18 },
                                   { "filter",required_argument,0,19 },
                                   { "fncount",required_argument,0,20 },
                                   { "grcount",required_argument,0,21 },
                                   { "help",no_argument,0,22 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 22*sizeof(int));
  shell_seen[15]=2;
  shell_seen[16]=2;
  shell_seen[17]=2;
  shell_seen[18]=2;
  shell_seen[19]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* cats */
      shell_read_vector(&cats, optarg);
      break;
    case 2: /* alpha */
      shell_read_real(&alpha, optarg);
      break;
    case 3: /* alpha-out */
      shell_arg_alpha=strdup(optarg);
      break;
    case 4: /* a */
      shell_read_real(&a, optarg);
      break;
    case 5: /* a-out */
      shell_arg_a=strdup(optarg);
      break;
    case 6: /* paralpha */
      shell_read_real(&paralpha, optarg);
      break;
    case 7: /* paralpha-out */
      shell_arg_paralpha=strdup(optarg);
      break;
    case 8: /* parbeta */
      shell_read_real(&parbeta, optarg);
      break;
    case 9: /* parbeta-out */
      shell_arg_parbeta=strdup(optarg);
      break;
    case 10: /* parscale */
      shell_read_real(&parscale, optarg);
      break;
    case 11: /* parscale-out */
      shell_arg_parscale=strdup(optarg);
      break;
    case 12: /* coeffs */
      shell_read_vector(&coeffs, optarg);
      break;
    case 13: /* coeffs-out */
      shell_arg_coeffs=strdup(optarg); 
  igraph_vector_init(&coeffs, 0);
      break;
    case 14: /* Fmin */
      shell_arg_Fmin=strdup(optarg);
      break;
    case 15: /* abstol */
      shell_read_real(&abstol, optarg);
      break;
    case 16: /* reltol */
      shell_read_real(&reltol, optarg);
      break;
    case 17: /* maxit */
      shell_read_int(&maxit, optarg);
      break;
    case 18: /* agebins */
      shell_read_int(&agebins, optarg);
      break;
    case 19: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 20: /* fncount */
      shell_arg_fncount=strdup(optarg);
      break;
    case 21: /* grcount */
      shell_arg_grcount=strdup(optarg);
      break;
    case 22:
      shell_igraph_revolver_ml_ADE_dpareto_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<22; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_ADE_dpareto(&graph, &cats, &alpha, &a, &paralpha, &parbeta, &parscale, &coeffs, &Fmin, abstol, reltol, maxit, agebins, filter, &fncount, &grcount);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&cats);
  shell_write_real(alpha, shell_arg_alpha);
  shell_write_real(a, shell_arg_a);
  shell_write_real(paralpha, shell_arg_paralpha);
  shell_write_real(parbeta, shell_arg_parbeta);
  shell_write_real(parscale, shell_arg_parscale);
  igraph_vector_destroy(&coeffs);
  shell_write_vector(&coeffs, shell_arg_coeffs); 
  igraph_vector_destroy(&coeffs);
  shell_write_real(Fmin, shell_arg_Fmin);
  if (filter) { igraph_vector_destroy(filter); }
  shell_write_integer(fncount, shell_arg_fncount);
  shell_write_integer(grcount, shell_arg_grcount);

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_ADE_dpareto_eval        /
/-------------------------------------------*/
void shell_igraph_revolver_ml_ADE_dpareto_eval_usage(char **argv) {
  printf("%s --graph=<graph> --cats=<cats> --alpha=<alpha> --a=<a> --paralpha=<paralpha> --parbeta=<parbeta> --parscale=<parscale> --coeffs=<coeffs> --value=<value> --deriv=<deriv> --agebins=<agebins> --filter=<filter>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_ADE_dpareto_eval(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t cats;
  igraph_real_t alpha;
  igraph_real_t a;
  igraph_real_t paralpha;
  igraph_real_t parbeta;
  igraph_real_t parscale;
  igraph_vector_t coeffs;
  igraph_real_t value;
  igraph_vector_t deriv;
  int agebins=300;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  char* shell_arg_value=0;
  char* shell_arg_deriv=0;
  int shell_result;


  int shell_seen[12];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "cats",required_argument,0,1 },
                                   { "alpha",required_argument,0,2 },
                                   { "a",required_argument,0,3 },
                                   { "paralpha",required_argument,0,4 },
                                   { "parbeta",required_argument,0,5 },
                                   { "parscale",required_argument,0,6 },
                                   { "coeffs",required_argument,0,7 },
                                   { "value",required_argument,0,8 },
                                   { "deriv",required_argument,0,9 },
                                   { "agebins",required_argument,0,10 },
                                   { "filter",required_argument,0,11 },
                                   { "help",no_argument,0,12 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 12*sizeof(int));
  shell_seen[10]=2;
  shell_seen[11]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* cats */
      shell_read_vector(&cats, optarg);
      break;
    case 2: /* alpha */
      shell_read_real(&alpha, optarg);
      break;
    case 3: /* a */
      shell_read_real(&a, optarg);
      break;
    case 4: /* paralpha */
      shell_read_real(&paralpha, optarg);
      break;
    case 5: /* parbeta */
      shell_read_real(&parbeta, optarg);
      break;
    case 6: /* parscale */
      shell_read_real(&parscale, optarg);
      break;
    case 7: /* coeffs */
      shell_read_vector(&coeffs, optarg);
      break;
    case 8: /* value */
      shell_arg_value=strdup(optarg);
      break;
    case 9: /* deriv */
      shell_arg_deriv=strdup(optarg); 
  igraph_vector_init(&deriv, 0);
      break;
    case 10: /* agebins */
      shell_read_int(&agebins, optarg);
      break;
    case 11: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 12:
      shell_igraph_revolver_ml_ADE_dpareto_eval_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<12; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_ADE_dpareto_eval(&graph, &cats, alpha, a, paralpha, parbeta, parscale, &coeffs, &value, &deriv, agebins, filter);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&cats);
  igraph_vector_destroy(&coeffs);
  shell_write_real(value, shell_arg_value);
  shell_write_vector(&deriv, shell_arg_deriv); 
  igraph_vector_destroy(&deriv);
  if (filter) { igraph_vector_destroy(filter); }

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_ml_ADE_dpareto_evalf       /
/-------------------------------------------*/
void shell_igraph_revolver_ml_ADE_dpareto_evalf_usage(char **argv) {
  printf("%s --graph=<graph> --cats=<cats> --par=<par> --value=<value> --agebins=<agebins> --filter=<filter>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_ml_ADE_dpareto_evalf(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t cats;
  igraph_matrix_t par;
  igraph_vector_t value;
  int agebins;
  igraph_vector_t v_filter; igraph_vector_t *filter=0=NULL;
  char* shell_arg_value=0;
  int shell_result;


  int shell_seen[6];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "cats",required_argument,0,1 },
                                   { "par",required_argument,0,2 },
                                   { "value",required_argument,0,3 },
                                   { "agebins",required_argument,0,4 },
                                   { "filter",required_argument,0,5 },
                                   { "help",no_argument,0,6 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 6*sizeof(int));
  shell_seen[5]=2;
  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* cats */
      shell_read_vector(&cats, optarg);
      break;
    case 2: /* par */
      shell_read_matrix(&par, optarg);
      break;
    case 3: /* value */
      shell_arg_value=strdup(optarg); 
  igraph_vector_init(&value, 0);
      break;
    case 4: /* agebins */
      shell_read_int(&agebins, optarg);
      break;
    case 5: /* filter */
      filter=&v_filter; shell_read_vector(filter, optarg);
      break;
    case 6:
      shell_igraph_revolver_ml_ADE_dpareto_evalf_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<6; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_ml_ADE_dpareto_evalf(&graph, &cats, &par, &value, agebins, filter);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_vector_destroy(&cats);
  igraph_matrix_destroy(&par);
  shell_write_vector(&value, shell_arg_value); 
  igraph_vector_destroy(&value);
  if (filter) { igraph_vector_destroy(filter); }

  return 0;
}

/*-------------------------------------------/
/ igraph_revolver_probs_ADE_dpareto          /
/-------------------------------------------*/
void shell_igraph_revolver_probs_ADE_dpareto_usage(char **argv) {
  printf("%s --graph=<graph> --par=<par> --cats=<cats> --gcats=<gcats> --agebins=<agebins> --logprobs=<logprobs> --logcited=<logcited> --logciting=<logciting>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_revolver_probs_ADE_dpareto(int argc, char **argv) {

  igraph_t graph;
  igraph_matrix_t par;
  igraph_vector_t cats;
  igraph_vector_t gcats;
  int agebins;
  igraph_vector_t v_logprobs; igraph_vector_t *logprobs=0;
  igraph_vector_t v_logcited; igraph_vector_t *logcited=0;
  igraph_vector_t v_logciting; igraph_vector_t *logciting=0;
  char* shell_arg_logprobs=0;
  char* shell_arg_logcited=0;
  char* shell_arg_logciting=0;
  int shell_result;


  int shell_seen[8];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "par",required_argument,0,1 },
                                   { "cats",required_argument,0,2 },
                                   { "gcats",required_argument,0,3 },
                                   { "agebins",required_argument,0,4 },
                                   { "logprobs",required_argument,0,5 },
                                   { "logcited",required_argument,0,6 },
                                   { "logciting",required_argument,0,7 },
                                   { "help",no_argument,0,8 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 8*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* par */
      shell_read_matrix(&par, optarg);
      break;
    case 2: /* cats */
      shell_read_vector(&cats, optarg);
      break;
    case 3: /* gcats */
      shell_read_vector(&gcats, optarg);
      break;
    case 4: /* agebins */
      shell_read_int(&agebins, optarg);
      break;
    case 5: /* logprobs */
      logprobs=&v_logprobs; igraph_vector_init(logprobs, 0); 
  shell_arg_logprobs=strdup(optarg);
      break;
    case 6: /* logcited */
      logcited=&v_logcited; igraph_vector_init(logcited, 0); 
  shell_arg_logcited=strdup(optarg);
      break;
    case 7: /* logciting */
      logciting=&v_logciting; igraph_vector_init(logciting, 0); 
  shell_arg_logciting=strdup(optarg);
      break;
    case 8:
      shell_igraph_revolver_probs_ADE_dpareto_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<8; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_revolver_probs_ADE_dpareto(&graph, &par, &cats, &gcats, agebins, logprobs, logcited, logciting);

  /* Write the result */
  igraph_destroy(&graph);
  igraph_matrix_destroy(&par);
  igraph_vector_destroy(&cats);
  igraph_vector_destroy(&gcats);
  if (logprobs) { shell_write_vector(logprobs, shell_arg_logprobs); 
  igraph_vector_destroy(logprobs); }
  if (logcited) { shell_write_vector(logcited, shell_arg_logcited); 
  igraph_vector_destroy(logcited); }
  if (logciting) { shell_write_vector(logciting, shell_arg_logciting); 
  igraph_vector_destroy(logciting); }

  return 0;
}

/*-------------------------------------------/
/ igraph_convergence_degree                  /
/-------------------------------------------*/
void shell_igraph_convergence_degree_usage(char **argv) {
  printf("%s --graph=<graph> --result=<result>\n", basename(argv[0]));
  exit(1);
}

int shell_igraph_convergence_degree(int argc, char **argv) {

  igraph_t graph;
  igraph_vector_t result;
  char* shell_arg_result=0;
  int shell_result;


  int shell_seen[2];
  int shell_index=-1;
  struct option shell_options[]= { { "graph",required_argument,0,0 },
                                   { "result",required_argument,0,1 },
                                   { "help",no_argument,0,2 },
                                   { 0,0,0,0 }
                                 };

  /* 0 - not seen, 1 - seen as argument, 2 - seen as default */
  memset(shell_seen, 0, 2*sizeof(int));

  
  /* Parse arguments and read input */
  while (getopt_long(argc, argv, "", shell_options, &shell_index) != -1) {

    if (shell_index==-1) {
      exit(1);
    }

    if (shell_seen[shell_index]==1) {
      fprintf(stderr, "Error, `--%s' argument given twice.\n",
              shell_options[shell_index].name);
      exit(1);
    }
    shell_seen[shell_index]=1;  

    switch (shell_index) {
    case 0: /* graph */
      shell_read_graph(&graph, optarg);
      break;
    case 1: /* result */
      shell_arg_result=strdup(optarg); 
  igraph_vector_init(&result, 0);
      break;
    case 2:
      shell_igraph_convergence_degree_usage(argv);
      break;
    default:
      break;
    }

    shell_index=-1;
  }

  /* Check that we have all arguments */
  for (shell_index=0; shell_index<2; shell_index++) {
    if (!shell_seen[shell_index]) {
      fprintf(stderr, "Error, argument missing: `--%s'.\n",
              shell_options[shell_index].name);
      exit(1);
    }
  }

  /* Do the operation */
  shell_result=igraph_convergence_degree(&graph, &result);

  /* Write the result */
  igraph_destroy(&graph);
  shell_write_vector(&result, shell_arg_result); 
  igraph_vector_destroy(&result);

  return 0;
}
