--
-- PostgreSQL database dump
--

-- Started on 2010-04-30 21:21:47

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;

--
-- TOC entry 313 (class 1247 OID 25371)
-- Dependencies: 6 1534
-- Name: tablefunc_crosstab_2; Type: TYPE; Schema: public; Owner: aurel.paulovic
--

CREATE TYPE tablefunc_crosstab_2 AS (
	row_name text,
	category_1 text,
	category_2 text
);


ALTER TYPE public.tablefunc_crosstab_2 OWNER TO "aurel.paulovic";

--
-- TOC entry 315 (class 1247 OID 25374)
-- Dependencies: 6 1535
-- Name: tablefunc_crosstab_3; Type: TYPE; Schema: public; Owner: aurel.paulovic
--

CREATE TYPE tablefunc_crosstab_3 AS (
	row_name text,
	category_1 text,
	category_2 text,
	category_3 text
);


ALTER TYPE public.tablefunc_crosstab_3 OWNER TO "aurel.paulovic";

--
-- TOC entry 317 (class 1247 OID 25377)
-- Dependencies: 6 1536
-- Name: tablefunc_crosstab_4; Type: TYPE; Schema: public; Owner: aurel.paulovic
--

CREATE TYPE tablefunc_crosstab_4 AS (
	row_name text,
	category_1 text,
	category_2 text,
	category_3 text,
	category_4 text
);


ALTER TYPE public.tablefunc_crosstab_4 OWNER TO "aurel.paulovic";

--
-- TOC entry 12 (class 1255 OID 25378)
-- Dependencies: 6
-- Name: connectby(text, text, text, text, integer, text); Type: FUNCTION; Schema: public; Owner: aurel.paulovic
--

CREATE FUNCTION connectby(text, text, text, text, integer, text) RETURNS SETOF record
    LANGUAGE c STABLE STRICT
    AS '$libdir/tablefunc', 'connectby_text';


ALTER FUNCTION public.connectby(text, text, text, text, integer, text) OWNER TO "aurel.paulovic";

--
-- TOC entry 20 (class 1255 OID 25379)
-- Dependencies: 6
-- Name: connectby(text, text, text, text, integer); Type: FUNCTION; Schema: public; Owner: aurel.paulovic
--

CREATE FUNCTION connectby(text, text, text, text, integer) RETURNS SETOF record
    LANGUAGE c STABLE STRICT
    AS '$libdir/tablefunc', 'connectby_text';


ALTER FUNCTION public.connectby(text, text, text, text, integer) OWNER TO "aurel.paulovic";

--
-- TOC entry 21 (class 1255 OID 25380)
-- Dependencies: 6
-- Name: connectby(text, text, text, text, text, integer, text); Type: FUNCTION; Schema: public; Owner: aurel.paulovic
--

CREATE FUNCTION connectby(text, text, text, text, text, integer, text) RETURNS SETOF record
    LANGUAGE c STABLE STRICT
    AS '$libdir/tablefunc', 'connectby_text_serial';


ALTER FUNCTION public.connectby(text, text, text, text, text, integer, text) OWNER TO "aurel.paulovic";

--
-- TOC entry 22 (class 1255 OID 25381)
-- Dependencies: 6
-- Name: connectby(text, text, text, text, text, integer); Type: FUNCTION; Schema: public; Owner: aurel.paulovic
--

CREATE FUNCTION connectby(text, text, text, text, text, integer) RETURNS SETOF record
    LANGUAGE c STABLE STRICT
    AS '$libdir/tablefunc', 'connectby_text_serial';


ALTER FUNCTION public.connectby(text, text, text, text, text, integer) OWNER TO "aurel.paulovic";

--
-- TOC entry 23 (class 1255 OID 25382)
-- Dependencies: 6
-- Name: crosstab(text); Type: FUNCTION; Schema: public; Owner: aurel.paulovic
--

CREATE FUNCTION crosstab(text) RETURNS SETOF record
    LANGUAGE c STABLE STRICT
    AS '$libdir/tablefunc', 'crosstab';


ALTER FUNCTION public.crosstab(text) OWNER TO "aurel.paulovic";

--
-- TOC entry 24 (class 1255 OID 25383)
-- Dependencies: 6
-- Name: crosstab(text, integer); Type: FUNCTION; Schema: public; Owner: aurel.paulovic
--

CREATE FUNCTION crosstab(text, integer) RETURNS SETOF record
    LANGUAGE c STABLE STRICT
    AS '$libdir/tablefunc', 'crosstab';


ALTER FUNCTION public.crosstab(text, integer) OWNER TO "aurel.paulovic";

--
-- TOC entry 25 (class 1255 OID 25384)
-- Dependencies: 6
-- Name: crosstab(text, text); Type: FUNCTION; Schema: public; Owner: aurel.paulovic
--

CREATE FUNCTION crosstab(text, text) RETURNS SETOF record
    LANGUAGE c STABLE STRICT
    AS '$libdir/tablefunc', 'crosstab_hash';


ALTER FUNCTION public.crosstab(text, text) OWNER TO "aurel.paulovic";

--
-- TOC entry 26 (class 1255 OID 25385)
-- Dependencies: 6 313
-- Name: crosstab2(text); Type: FUNCTION; Schema: public; Owner: aurel.paulovic
--

CREATE FUNCTION crosstab2(text) RETURNS SETOF tablefunc_crosstab_2
    LANGUAGE c STABLE STRICT
    AS '$libdir/tablefunc', 'crosstab';


ALTER FUNCTION public.crosstab2(text) OWNER TO "aurel.paulovic";

--
-- TOC entry 27 (class 1255 OID 25386)
-- Dependencies: 315 6
-- Name: crosstab3(text); Type: FUNCTION; Schema: public; Owner: aurel.paulovic
--

CREATE FUNCTION crosstab3(text) RETURNS SETOF tablefunc_crosstab_3
    LANGUAGE c STABLE STRICT
    AS '$libdir/tablefunc', 'crosstab';


ALTER FUNCTION public.crosstab3(text) OWNER TO "aurel.paulovic";

--
-- TOC entry 28 (class 1255 OID 25387)
-- Dependencies: 6 317
-- Name: crosstab4(text); Type: FUNCTION; Schema: public; Owner: aurel.paulovic
--

CREATE FUNCTION crosstab4(text) RETURNS SETOF tablefunc_crosstab_4
    LANGUAGE c STABLE STRICT
    AS '$libdir/tablefunc', 'crosstab';


ALTER FUNCTION public.crosstab4(text) OWNER TO "aurel.paulovic";

--
-- TOC entry 29 (class 1255 OID 25388)
-- Dependencies: 6
-- Name: normal_rand(integer, double precision, double precision); Type: FUNCTION; Schema: public; Owner: aurel.paulovic
--

CREATE FUNCTION normal_rand(integer, double precision, double precision) RETURNS SETOF double precision
    LANGUAGE c STRICT
    AS '$libdir/tablefunc', 'normal_rand';


ALTER FUNCTION public.normal_rand(integer, double precision, double precision) OWNER TO "aurel.paulovic";

SET default_tablespace = '';

SET default_with_oids = false;

--
-- TOC entry 1548 (class 1259 OID 25625)
-- Dependencies: 6
-- Name: edge_settings; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE TABLE edge_settings (
    graph_id bigint NOT NULL,
    edge_id bigint NOT NULL,
    val_name character varying NOT NULL,
    val character varying
);


ALTER TABLE public.edge_settings OWNER TO "aurel.paulovic";

--
-- TOC entry 1537 (class 1259 OID 25389)
-- Dependencies: 1826 6
-- Name: edges; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE TABLE edges (
    edge_id bigint NOT NULL,
    name character varying,
    type_id bigint NOT NULL,
    n1 bigint NOT NULL,
    n2 bigint NOT NULL,
    oriented boolean DEFAULT true NOT NULL,
    graph_id integer NOT NULL
);


ALTER TABLE public.edges OWNER TO "aurel.paulovic";

--
-- TOC entry 1538 (class 1259 OID 25396)
-- Dependencies: 6
-- Name: element_id_seq; Type: SEQUENCE; Schema: public; Owner: aurel.paulovic
--

CREATE SEQUENCE element_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.element_id_seq OWNER TO "aurel.paulovic";

--
-- TOC entry 1892 (class 0 OID 0)
-- Dependencies: 1538
-- Name: element_id_seq; Type: SEQUENCE SET; Schema: public; Owner: aurel.paulovic
--

SELECT pg_catalog.setval('element_id_seq', 292, true);


--
-- TOC entry 1539 (class 1259 OID 25398)
-- Dependencies: 6
-- Name: graph_id_seq; Type: SEQUENCE; Schema: public; Owner: aurel.paulovic
--

CREATE SEQUENCE graph_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.graph_id_seq OWNER TO "aurel.paulovic";

--
-- TOC entry 1894 (class 0 OID 0)
-- Dependencies: 1539
-- Name: graph_id_seq; Type: SEQUENCE SET; Schema: public; Owner: aurel.paulovic
--

SELECT pg_catalog.setval('graph_id_seq', 578, true);


--
-- TOC entry 1545 (class 1259 OID 25572)
-- Dependencies: 6
-- Name: graph_settings; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE TABLE graph_settings (
    graph_id bigint NOT NULL,
    val_name character varying NOT NULL,
    val character varying
);


ALTER TABLE public.graph_settings OWNER TO "aurel.paulovic";

--
-- TOC entry 1540 (class 1259 OID 25400)
-- Dependencies: 1827 6
-- Name: graphs; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE TABLE graphs (
    graph_id integer DEFAULT nextval('graph_id_seq'::regclass) NOT NULL,
    graph_name character varying
);


ALTER TABLE public.graphs OWNER TO "aurel.paulovic";

--
-- TOC entry 1541 (class 1259 OID 25407)
-- Dependencies: 6
-- Name: layout_id_seq; Type: SEQUENCE; Schema: public; Owner: aurel.paulovic
--

CREATE SEQUENCE layout_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.layout_id_seq OWNER TO "aurel.paulovic";

--
-- TOC entry 1898 (class 0 OID 0)
-- Dependencies: 1541
-- Name: layout_id_seq; Type: SEQUENCE SET; Schema: public; Owner: aurel.paulovic
--

SELECT pg_catalog.setval('layout_id_seq', 240, true);


--
-- TOC entry 1546 (class 1259 OID 25598)
-- Dependencies: 6
-- Name: layout_settings; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE TABLE layout_settings (
    graph_id bigint NOT NULL,
    layout_id bigint NOT NULL,
    val_name character varying NOT NULL,
    val character varying
);


ALTER TABLE public.layout_settings OWNER TO "aurel.paulovic";

--
-- TOC entry 1542 (class 1259 OID 25409)
-- Dependencies: 1828 6
-- Name: layouts; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE TABLE layouts (
    graph_id integer NOT NULL,
    layout_id integer DEFAULT nextval('layout_id_seq'::regclass) NOT NULL,
    layout_name character varying
);


ALTER TABLE public.layouts OWNER TO "aurel.paulovic";

--
-- TOC entry 1547 (class 1259 OID 25612)
-- Dependencies: 6
-- Name: node_settings; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE TABLE node_settings (
    graph_id bigint NOT NULL,
    node_id bigint NOT NULL,
    val_name character varying NOT NULL,
    val character varying
);


ALTER TABLE public.node_settings OWNER TO "aurel.paulovic";

--
-- TOC entry 1543 (class 1259 OID 25419)
-- Dependencies: 1829 1830 1831 1832 6
-- Name: nodes; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE TABLE nodes (
    node_id bigint DEFAULT nextval('element_id_seq'::regclass) NOT NULL,
    name character varying,
    type_id bigint DEFAULT currval('element_id_seq'::regclass) NOT NULL,
    graph_id integer NOT NULL,
    meta boolean DEFAULT false NOT NULL,
    layout_id bigint,
    CONSTRAINT nodes_only_meta_has_layout_check CHECK ((((meta = true) AND (layout_id IS NOT NULL)) OR ((meta = false) AND (layout_id IS NULL))))
);


ALTER TABLE public.nodes OWNER TO "aurel.paulovic";

--
-- TOC entry 1903 (class 0 OID 0)
-- Dependencies: 1543
-- Name: CONSTRAINT nodes_only_meta_has_layout_check ON nodes; Type: COMMENT; Schema: public; Owner: aurel.paulovic
--

COMMENT ON CONSTRAINT nodes_only_meta_has_layout_check ON nodes IS 'only meta types can have layout_id';


--
-- TOC entry 1544 (class 1259 OID 25428)
-- Dependencies: 6
-- Name: positions; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE TABLE positions (
    layout_id integer NOT NULL,
    node_id bigint NOT NULL,
    pos_x double precision NOT NULL,
    pos_y double precision NOT NULL,
    pos_z double precision NOT NULL,
    graph_id bigint NOT NULL
);


ALTER TABLE public.positions OWNER TO "aurel.paulovic";

--
-- TOC entry 1884 (class 0 OID 25625)
-- Dependencies: 1548
-- Data for Name: edge_settings; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--



--
-- TOC entry 1876 (class 0 OID 25389)
-- Dependencies: 1537
-- Data for Name: edges; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--



--
-- TOC entry 1881 (class 0 OID 25572)
-- Dependencies: 1545
-- Data for Name: graph_settings; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--

INSERT INTO graph_settings (graph_id, val_name, val) VALUES (392, 'name', 'circle');
INSERT INTO graph_settings (graph_id, val_name, val) VALUES (393, 'name', 'simple');
INSERT INTO graph_settings (graph_id, val_name, val) VALUES (394, 'name', 'colored');
INSERT INTO graph_settings (graph_id, val_name, val) VALUES (392, 'layout_count', '5');
INSERT INTO graph_settings (graph_id, val_name, val) VALUES (393, 'file_size', '65kb');
INSERT INTO graph_settings (graph_id, val_name, val) VALUES (394, 'file_size', '1kb');
INSERT INTO graph_settings (graph_id, val_name, val) VALUES (394, 'path', 'resources/colored.graphml');


--
-- TOC entry 1877 (class 0 OID 25400)
-- Dependencies: 1540
-- Data for Name: graphs; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--

INSERT INTO graphs (graph_id, graph_name) VALUES (392, 'graph1');
INSERT INTO graphs (graph_id, graph_name) VALUES (393, 'graph2');
INSERT INTO graphs (graph_id, graph_name) VALUES (394, 'TODOpridajNazovGrafu');
INSERT INTO graphs (graph_id, graph_name) VALUES (395, 'TODOpridajNazovGrafu');
INSERT INTO graphs (graph_id, graph_name) VALUES (396, 'TODOpridajNazovGrafu');
INSERT INTO graphs (graph_id, graph_name) VALUES (397, 'Graph G');
INSERT INTO graphs (graph_id, graph_name) VALUES (398, 'Graph G');
INSERT INTO graphs (graph_id, graph_name) VALUES (406, 'Graph G');
INSERT INTO graphs (graph_id, graph_name) VALUES (576, 'Graph G');
INSERT INTO graphs (graph_id, graph_name) VALUES (577, 'Graph G');
INSERT INTO graphs (graph_id, graph_name) VALUES (578, 'Graph G');


--
-- TOC entry 1882 (class 0 OID 25598)
-- Dependencies: 1546
-- Data for Name: layout_settings; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--



--
-- TOC entry 1878 (class 0 OID 25409)
-- Dependencies: 1542
-- Data for Name: layouts; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--

INSERT INTO layouts (graph_id, layout_id, layout_name) VALUES (392, 20, 'layout1');
INSERT INTO layouts (graph_id, layout_id, layout_name) VALUES (392, 21, 'layout2');
INSERT INTO layouts (graph_id, layout_id, layout_name) VALUES (392, 22, 'new Layout');
INSERT INTO layouts (graph_id, layout_id, layout_name) VALUES (397, 44, 'new Layout');
INSERT INTO layouts (graph_id, layout_id, layout_name) VALUES (398, 45, 'new Layout');
INSERT INTO layouts (graph_id, layout_id, layout_name) VALUES (406, 59, 'new Layout');
INSERT INTO layouts (graph_id, layout_id, layout_name) VALUES (576, 238, 'new Layout');
INSERT INTO layouts (graph_id, layout_id, layout_name) VALUES (577, 239, 'new Layout');
INSERT INTO layouts (graph_id, layout_id, layout_name) VALUES (578, 240, 'new Layout');


--
-- TOC entry 1883 (class 0 OID 25612)
-- Dependencies: 1547
-- Data for Name: node_settings; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--



--
-- TOC entry 1879 (class 0 OID 25419)
-- Dependencies: 1543
-- Data for Name: nodes; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--



--
-- TOC entry 1880 (class 0 OID 25428)
-- Dependencies: 1544
-- Data for Name: positions; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--



--
-- TOC entry 1859 (class 2606 OID 25641)
-- Dependencies: 1548 1548 1548 1548
-- Name: edge_settings_primary_key; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace:
--

ALTER TABLE ONLY edge_settings
    ADD CONSTRAINT edge_settings_primary_key PRIMARY KEY (graph_id, edge_id, val_name);


--
-- TOC entry 1834 (class 2606 OID 25432)
-- Dependencies: 1537 1537 1537
-- Name: edges_primary; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace:
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_primary PRIMARY KEY (edge_id, graph_id);


--
-- TOC entry 1853 (class 2606 OID 25639)
-- Dependencies: 1545 1545 1545
-- Name: graph_settings_primary; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace:
--

ALTER TABLE ONLY graph_settings
    ADD CONSTRAINT graph_settings_primary PRIMARY KEY (graph_id, val_name);


--
-- TOC entry 1840 (class 2606 OID 25434)
-- Dependencies: 1540 1540
-- Name: graphs_graph_id_primary; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace:
--

ALTER TABLE ONLY graphs
    ADD CONSTRAINT graphs_graph_id_primary PRIMARY KEY (graph_id);


--
-- TOC entry 1855 (class 2606 OID 25659)
-- Dependencies: 1546 1546 1546 1546
-- Name: layout_settings_primary_key; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace:
--

ALTER TABLE ONLY layout_settings
    ADD CONSTRAINT layout_settings_primary_key PRIMARY KEY (graph_id, layout_id, val_name);


--
-- TOC entry 1842 (class 2606 OID 25506)
-- Dependencies: 1542 1542 1542
-- Name: layouts_primary; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace:
--

ALTER TABLE ONLY layouts
    ADD CONSTRAINT layouts_primary PRIMARY KEY (graph_id, layout_id);


--
-- TOC entry 1857 (class 2606 OID 25672)
-- Dependencies: 1547 1547 1547 1547
-- Name: node_settings_primary_key; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace:
--

ALTER TABLE ONLY node_settings
    ADD CONSTRAINT node_settings_primary_key PRIMARY KEY (graph_id, node_id, val_name);


--
-- TOC entry 1848 (class 2606 OID 25508)
-- Dependencies: 1543 1543 1543
-- Name: nodes_primary; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace:
--

ALTER TABLE ONLY nodes
    ADD CONSTRAINT nodes_primary PRIMARY KEY (node_id, graph_id);


--
-- TOC entry 1851 (class 2606 OID 25532)
-- Dependencies: 1544 1544 1544 1544
-- Name: positions_primary; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace:
--

ALTER TABLE ONLY positions
    ADD CONSTRAINT positions_primary PRIMARY KEY (graph_id, layout_id, node_id);


--
-- TOC entry 1860 (class 1259 OID 25670)
-- Dependencies: 1548 1548
-- Name: fki_edge_settings_graph_edge_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE INDEX fki_edge_settings_graph_edge_fk ON edge_settings USING btree (edge_id, graph_id);


--
-- TOC entry 1835 (class 1259 OID 25443)
-- Dependencies: 1537
-- Name: fki_edges_graph_id_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE INDEX fki_edges_graph_id_fk ON edges USING btree (graph_id);


--
-- TOC entry 1836 (class 1259 OID 25559)
-- Dependencies: 1537 1537
-- Name: fki_edges_n1_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE INDEX fki_edges_n1_fk ON edges USING btree (n1, graph_id);


--
-- TOC entry 1837 (class 1259 OID 25565)
-- Dependencies: 1537 1537
-- Name: fki_edges_n2_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE INDEX fki_edges_n2_fk ON edges USING btree (n2, graph_id);


--
-- TOC entry 1838 (class 1259 OID 25571)
-- Dependencies: 1537 1537
-- Name: fki_edges_type_id_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE INDEX fki_edges_type_id_fk ON edges USING btree (graph_id, type_id);


--
-- TOC entry 1843 (class 1259 OID 25447)
-- Dependencies: 1543
-- Name: fki_nodes_graph_id_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE INDEX fki_nodes_graph_id_fk ON nodes USING btree (graph_id);


--
-- TOC entry 1844 (class 1259 OID 25529)
-- Dependencies: 1543 1543
-- Name: fki_nodes_layout_id_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE INDEX fki_nodes_layout_id_fk ON nodes USING btree (graph_id, layout_id);


--
-- TOC entry 1845 (class 1259 OID 25523)
-- Dependencies: 1543 1543
-- Name: fki_nodes_type_id_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE INDEX fki_nodes_type_id_fk ON nodes USING btree (graph_id, type_id);


--
-- TOC entry 1849 (class 1259 OID 25548)
-- Dependencies: 1544 1544
-- Name: fki_positions_node_id_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE INDEX fki_positions_node_id_fk ON positions USING btree (node_id, graph_id);


--
-- TOC entry 1846 (class 1259 OID 25512)
-- Dependencies: 1543 1543 1543 1543
-- Name: nodes_node_equals_type; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace:
--

CREATE INDEX nodes_node_equals_type ON nodes USING btree (node_id, graph_id) WHERE (node_id = type_id);


--
-- TOC entry 1875 (class 2606 OID 25665)
-- Dependencies: 1537 1537 1548 1548 1833
-- Name: edge_settings_graph_edge_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY edge_settings
    ADD CONSTRAINT edge_settings_graph_edge_fk FOREIGN KEY (edge_id, graph_id) REFERENCES edges(edge_id, graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1861 (class 2606 OID 25450)
-- Dependencies: 1839 1540 1537
-- Name: edges_graph_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_graph_id_fk FOREIGN KEY (graph_id) REFERENCES graphs(graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1862 (class 2606 OID 25554)
-- Dependencies: 1847 1537 1537 1543 1543
-- Name: edges_n1_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_n1_fk FOREIGN KEY (n1, graph_id) REFERENCES nodes(node_id, graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1863 (class 2606 OID 25560)
-- Dependencies: 1537 1543 1847 1537 1543
-- Name: edges_n2_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_n2_fk FOREIGN KEY (n2, graph_id) REFERENCES nodes(node_id, graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1864 (class 2606 OID 25566)
-- Dependencies: 1537 1543 1543 1537 1847
-- Name: edges_type_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_type_id_fk FOREIGN KEY (graph_id, type_id) REFERENCES nodes(graph_id, node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1872 (class 2606 OID 25653)
-- Dependencies: 1540 1545 1839
-- Name: graph_settings_graph_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY graph_settings
    ADD CONSTRAINT graph_settings_graph_fk FOREIGN KEY (graph_id) REFERENCES graphs(graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1873 (class 2606 OID 25660)
-- Dependencies: 1542 1841 1546 1542 1546
-- Name: layout_settings_graph_layout_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY layout_settings
    ADD CONSTRAINT layout_settings_graph_layout_fk FOREIGN KEY (graph_id, layout_id) REFERENCES layouts(graph_id, layout_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1865 (class 2606 OID 25470)
-- Dependencies: 1542 1540 1839
-- Name: layouts_graph_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY layouts
    ADD CONSTRAINT layouts_graph_id_fk FOREIGN KEY (graph_id) REFERENCES graphs(graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1874 (class 2606 OID 25620)
-- Dependencies: 1547 1547 1543 1543 1847
-- Name: node_settings_graph_node_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY node_settings
    ADD CONSTRAINT node_settings_graph_node_id_fk FOREIGN KEY (graph_id, node_id) REFERENCES nodes(graph_id, node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1866 (class 2606 OID 25485)
-- Dependencies: 1543 1540 1839
-- Name: nodes_graph_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY nodes
    ADD CONSTRAINT nodes_graph_id_fk FOREIGN KEY (graph_id) REFERENCES graphs(graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1868 (class 2606 OID 25524)
-- Dependencies: 1543 1543 1542 1542 1841
-- Name: nodes_layout_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY nodes
    ADD CONSTRAINT nodes_layout_id_fk FOREIGN KEY (graph_id, layout_id) REFERENCES layouts(graph_id, layout_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1867 (class 2606 OID 25518)
-- Dependencies: 1543 1543 1543 1543 1847
-- Name: nodes_type_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY nodes
    ADD CONSTRAINT nodes_type_id_fk FOREIGN KEY (graph_id, type_id) REFERENCES nodes(graph_id, node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1870 (class 2606 OID 25538)
-- Dependencies: 1839 1544 1540
-- Name: positions_graph_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY positions
    ADD CONSTRAINT positions_graph_id_fk FOREIGN KEY (graph_id) REFERENCES graphs(graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1869 (class 2606 OID 25533)
-- Dependencies: 1544 1841 1542 1542 1544
-- Name: positions_layout_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY positions
    ADD CONSTRAINT positions_layout_id_fk FOREIGN KEY (graph_id, layout_id) REFERENCES layouts(graph_id, layout_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1871 (class 2606 OID 25543)
-- Dependencies: 1544 1543 1543 1847 1544
-- Name: positions_node_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY positions
    ADD CONSTRAINT positions_node_id_fk FOREIGN KEY (node_id, graph_id) REFERENCES nodes(node_id, graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1889 (class 0 OID 0)
-- Dependencies: 6
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO "peter.kapec";
GRANT ALL ON SCHEMA public TO "aurel.paulovic";


--
-- TOC entry 1890 (class 0 OID 0)
-- Dependencies: 1548
-- Name: edge_settings; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE edge_settings FROM PUBLIC;
REVOKE ALL ON TABLE edge_settings FROM "aurel.paulovic";
GRANT ALL ON TABLE edge_settings TO "aurel.paulovic";
GRANT ALL ON TABLE edge_settings TO "peter.kapec";


--
-- TOC entry 1891 (class 0 OID 0)
-- Dependencies: 1537
-- Name: edges; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE edges FROM PUBLIC;
REVOKE ALL ON TABLE edges FROM "aurel.paulovic";
GRANT ALL ON TABLE edges TO "aurel.paulovic";
GRANT ALL ON TABLE edges TO "peter.kapec";


--
-- TOC entry 1893 (class 0 OID 0)
-- Dependencies: 1538
-- Name: element_id_seq; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON SEQUENCE element_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE element_id_seq FROM "aurel.paulovic";
GRANT SELECT,UPDATE ON SEQUENCE element_id_seq TO "aurel.paulovic";
GRANT SELECT,UPDATE ON SEQUENCE element_id_seq TO "peter.kapec";


--
-- TOC entry 1895 (class 0 OID 0)
-- Dependencies: 1539
-- Name: graph_id_seq; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON SEQUENCE graph_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE graph_id_seq FROM "aurel.paulovic";
GRANT ALL ON SEQUENCE graph_id_seq TO "aurel.paulovic";
GRANT SELECT,UPDATE ON SEQUENCE graph_id_seq TO "peter.kapec";


--
-- TOC entry 1896 (class 0 OID 0)
-- Dependencies: 1545
-- Name: graph_settings; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE graph_settings FROM PUBLIC;
REVOKE ALL ON TABLE graph_settings FROM "aurel.paulovic";
GRANT ALL ON TABLE graph_settings TO "aurel.paulovic";
GRANT ALL ON TABLE graph_settings TO "peter.kapec";


--
-- TOC entry 1897 (class 0 OID 0)
-- Dependencies: 1540
-- Name: graphs; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE graphs FROM PUBLIC;
REVOKE ALL ON TABLE graphs FROM "aurel.paulovic";
GRANT ALL ON TABLE graphs TO "aurel.paulovic";
GRANT ALL ON TABLE graphs TO "peter.kapec";


--
-- TOC entry 1899 (class 0 OID 0)
-- Dependencies: 1541
-- Name: layout_id_seq; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON SEQUENCE layout_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE layout_id_seq FROM "aurel.paulovic";
GRANT ALL ON SEQUENCE layout_id_seq TO "aurel.paulovic";
GRANT SELECT,UPDATE ON SEQUENCE layout_id_seq TO "peter.kapec";


--
-- TOC entry 1900 (class 0 OID 0)
-- Dependencies: 1546
-- Name: layout_settings; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE layout_settings FROM PUBLIC;
REVOKE ALL ON TABLE layout_settings FROM "aurel.paulovic";
GRANT ALL ON TABLE layout_settings TO "aurel.paulovic";
GRANT ALL ON TABLE layout_settings TO "peter.kapec";


--
-- TOC entry 1901 (class 0 OID 0)
-- Dependencies: 1542
-- Name: layouts; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE layouts FROM PUBLIC;
REVOKE ALL ON TABLE layouts FROM "aurel.paulovic";
GRANT ALL ON TABLE layouts TO "aurel.paulovic";
GRANT ALL ON TABLE layouts TO "peter.kapec";


--
-- TOC entry 1902 (class 0 OID 0)
-- Dependencies: 1547
-- Name: node_settings; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE node_settings FROM PUBLIC;
REVOKE ALL ON TABLE node_settings FROM "aurel.paulovic";
GRANT ALL ON TABLE node_settings TO "aurel.paulovic";
GRANT ALL ON TABLE node_settings TO "peter.kapec";


--
-- TOC entry 1904 (class 0 OID 0)
-- Dependencies: 1543
-- Name: nodes; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE nodes FROM PUBLIC;
REVOKE ALL ON TABLE nodes FROM "aurel.paulovic";
GRANT ALL ON TABLE nodes TO "aurel.paulovic";
GRANT ALL ON TABLE nodes TO "peter.kapec";


--
-- TOC entry 1905 (class 0 OID 0)
-- Dependencies: 1544
-- Name: positions; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE positions FROM PUBLIC;
REVOKE ALL ON TABLE positions FROM "aurel.paulovic";
GRANT ALL ON TABLE positions TO "aurel.paulovic";
GRANT ALL ON TABLE positions TO "peter.kapec";


-- Completed on 2010-04-30 21:21:48

--
-- PostgreSQL database dump complete
--

