--
-- PostgreSQL database dump
--

-- Started on 2010-03-14 21:31:31

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;


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
-- TOC entry 1507 (class 1259 OID 16579)
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
-- TOC entry 1838 (class 0 OID 0)
-- Dependencies: 1507
-- Name: element_id_seq; Type: SEQUENCE SET; Schema: public; Owner: aurel.paulovic
--

SELECT pg_catalog.setval('element_id_seq', 272, true);


--
-- TOC entry 1511 (class 1259 OID 24658)
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
-- TOC entry 1840 (class 0 OID 0)
-- Dependencies: 1511
-- Name: graph_id_seq; Type: SEQUENCE SET; Schema: public; Owner: aurel.paulovic
--

SELECT pg_catalog.setval('graph_id_seq', 14, true);


--
-- TOC entry 1512 (class 1259 OID 24660)
-- Dependencies: 1795 6
-- Name: graphs; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

CREATE TABLE graphs (
    graph_id integer DEFAULT nextval('graph_id_seq'::regclass) NOT NULL,
    graph_name character varying NOT NULL
);


ALTER TABLE public.graphs OWNER TO "aurel.paulovic";

--
-- TOC entry 1513 (class 1259 OID 24691)
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
-- TOC entry 1843 (class 0 OID 0)
-- Dependencies: 1513
-- Name: layout_id_seq; Type: SEQUENCE SET; Schema: public; Owner: aurel.paulovic
--

SELECT pg_catalog.setval('layout_id_seq', 7, true);


--
-- TOC entry 1514 (class 1259 OID 24693)
-- Dependencies: 1796 6
-- Name: layouts; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

CREATE TABLE layouts (
    graph_id integer NOT NULL,
    layout_id integer DEFAULT nextval('layout_id_seq'::regclass) NOT NULL,
    layout_name character varying NOT NULL
);


ALTER TABLE public.layouts OWNER TO "aurel.paulovic";

--
-- TOC entry 1515 (class 1259 OID 24743)
-- Dependencies: 6
-- Name: metatypes; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

CREATE TABLE metatypes (
    graph_id integer NOT NULL,
    layout_id integer NOT NULL,
    type_id integer NOT NULL
);


ALTER TABLE public.metatypes OWNER TO "aurel.paulovic";

--
-- TOC entry 1509 (class 1259 OID 16589)
-- Dependencies: 1794 6
-- Name: nodes; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

CREATE TABLE nodes (
    node_id bigint NOT NULL,
    name character varying,
    type_id bigint DEFAULT currval('element_id_seq'::regclass) NOT NULL,
    graph_id integer NOT NULL
);


ALTER TABLE public.nodes OWNER TO "aurel.paulovic";

--
-- TOC entry 1510 (class 1259 OID 16597)
-- Dependencies: 6
-- Name: positions; Type: TABLE; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

CREATE TABLE positions (
    layout_id integer NOT NULL,
    node_id bigint NOT NULL,
    pos_x double precision NOT NULL,
    pos_y double precision NOT NULL,
    pos_z double precision NOT NULL
);


ALTER TABLE public.positions OWNER TO "aurel.paulovic";

--
-- TOC entry 1826 (class 0 OID 16581)
-- Dependencies: 1508
-- Data for Name: edges; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--

INSERT INTO edges (edge_id, name, type_id, n1, n2, oriented, graph_id) VALUES (272, 'edge1', 269, 270, 271, true, 1);


--
-- TOC entry 1829 (class 0 OID 24660)
-- Dependencies: 1512
-- Data for Name: graphs; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--

INSERT INTO graphs (graph_id, graph_name) VALUES (1, 'graph1');
INSERT INTO graphs (graph_id, graph_name) VALUES (2, 'graph2');
INSERT INTO graphs (graph_id, graph_name) VALUES (3, 'graph3');


--
-- TOC entry 1830 (class 0 OID 24693)
-- Dependencies: 1514
-- Data for Name: layouts; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--

INSERT INTO layouts (graph_id, layout_id, layout_name) VALUES (1, 1, 'layout1');
INSERT INTO layouts (graph_id, layout_id, layout_name) VALUES (1, 3, 'layout2');


--
-- TOC entry 1831 (class 0 OID 24743)
-- Dependencies: 1515
-- Data for Name: metatypes; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--



--
-- TOC entry 1827 (class 0 OID 16589)
-- Dependencies: 1509
-- Data for Name: nodes; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--

INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (248, 'type1', 248, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (249, 'node1', 248, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (250, 'node2', 248, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (251, 'type1', 251, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (252, 'node1', 251, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (253, 'node2', 251, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (254, 'type1', 254, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (255, 'type1', 255, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (256, 'node1', 255, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (257, 'type1', 257, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (258, 'node1', 257, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (259, 'node2', 257, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (260, 'type1', 260, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (261, 'node1', 260, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (262, 'node2', 260, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (263, 'type1', 263, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (264, 'node1', 263, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (265, 'node2', 263, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (266, 'type1', 266, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (267, 'node1', 266, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (268, 'node2', 266, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (269, 'type1', 269, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (270, 'node1', 269, 1);
INSERT INTO nodes (node_id, name, type_id, graph_id) VALUES (271, 'node2', 269, 1);


--
-- TOC entry 1828 (class 0 OID 16597)
-- Dependencies: 1510
-- Data for Name: positions; Type: TABLE DATA; Schema: public; Owner: aurel.paulovic
--

INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 248, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 249, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 250, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 251, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 252, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 253, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 254, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 255, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 256, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 257, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 258, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 259, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 260, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 261, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 262, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 263, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 264, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 265, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 266, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 267, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 268, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 269, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 270, 0, 0, 0);
INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z) VALUES (1, 271, 0, 0, 0);


--
-- TOC entry 1798 (class 2606 OID 24716)
-- Dependencies: 1508 1508 1508
-- Name: edges_primary; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_primary PRIMARY KEY (edge_id, graph_id);


--
-- TOC entry 1810 (class 2606 OID 24668)
-- Dependencies: 1512 1512
-- Name: graphs_graph_id_primary; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

ALTER TABLE ONLY graphs
    ADD CONSTRAINT graphs_graph_id_primary PRIMARY KEY (graph_id);


--
-- TOC entry 1812 (class 2606 OID 24701)
-- Dependencies: 1514 1514
-- Name: layouts_primary; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

ALTER TABLE ONLY layouts
    ADD CONSTRAINT layouts_primary PRIMARY KEY (layout_id);


--
-- TOC entry 1814 (class 2606 OID 24747)
-- Dependencies: 1515 1515 1515 1515
-- Name: metatypes_primary; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

ALTER TABLE ONLY metatypes
    ADD CONSTRAINT metatypes_primary PRIMARY KEY (graph_id, layout_id, type_id);


--
-- TOC entry 1806 (class 2606 OID 16604)
-- Dependencies: 1509 1509
-- Name: ndoes_node_id_pk; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

ALTER TABLE ONLY nodes
    ADD CONSTRAINT ndoes_node_id_pk PRIMARY KEY (node_id);


--
-- TOC entry 1808 (class 2606 OID 24708)
-- Dependencies: 1510 1510 1510
-- Name: positions_primary; Type: CONSTRAINT; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

ALTER TABLE ONLY positions
    ADD CONSTRAINT positions_primary PRIMARY KEY (layout_id, node_id);


--
-- TOC entry 1799 (class 1259 OID 24714)
-- Dependencies: 1508
-- Name: fki_edges_graph_id_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

CREATE INDEX fki_edges_graph_id_fk ON edges USING btree (graph_id);


--
-- TOC entry 1800 (class 1259 OID 16607)
-- Dependencies: 1508
-- Name: fki_edges_n1_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

CREATE INDEX fki_edges_n1_fk ON edges USING btree (n1);


--
-- TOC entry 1801 (class 1259 OID 16608)
-- Dependencies: 1508
-- Name: fki_edges_n2_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

CREATE INDEX fki_edges_n2_fk ON edges USING btree (n2);


--
-- TOC entry 1802 (class 1259 OID 16609)
-- Dependencies: 1508
-- Name: fki_edges_type_id_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

CREATE INDEX fki_edges_type_id_fk ON edges USING btree (type_id);


--
-- TOC entry 1803 (class 1259 OID 24742)
-- Dependencies: 1509
-- Name: fki_nodes_graph_id_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

CREATE INDEX fki_nodes_graph_id_fk ON nodes USING btree (graph_id);


--
-- TOC entry 1804 (class 1259 OID 16610)
-- Dependencies: 1509
-- Name: fki_nodes_type_id_fk; Type: INDEX; Schema: public; Owner: aurel.paulovic; Tablespace: 
--

CREATE INDEX fki_nodes_type_id_fk ON nodes USING btree (type_id);


--
-- TOC entry 1818 (class 2606 OID 24709)
-- Dependencies: 1512 1809 1508
-- Name: edges_graph_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_graph_id_fk FOREIGN KEY (graph_id) REFERENCES graphs(graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1815 (class 2606 OID 16611)
-- Dependencies: 1805 1509 1508
-- Name: edges_n1_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_n1_fk FOREIGN KEY (n1) REFERENCES nodes(node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1816 (class 2606 OID 16616)
-- Dependencies: 1508 1509 1805
-- Name: edges_n2_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_n2_fk FOREIGN KEY (n2) REFERENCES nodes(node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1817 (class 2606 OID 16621)
-- Dependencies: 1805 1508 1509
-- Name: edges_type_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_type_id_fk FOREIGN KEY (type_id) REFERENCES nodes(node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1823 (class 2606 OID 24717)
-- Dependencies: 1809 1514 1512
-- Name: layouts_graph_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY layouts
    ADD CONSTRAINT layouts_graph_id_fk FOREIGN KEY (graph_id) REFERENCES graphs(graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1824 (class 2606 OID 24748)
-- Dependencies: 1514 1811 1515
-- Name: metatypes_layout_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY metatypes
    ADD CONSTRAINT metatypes_layout_id_fk FOREIGN KEY (layout_id) REFERENCES layouts(layout_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1825 (class 2606 OID 24753)
-- Dependencies: 1515 1805 1509
-- Name: metatypes_type_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY metatypes
    ADD CONSTRAINT metatypes_type_id_fk FOREIGN KEY (type_id) REFERENCES nodes(node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1820 (class 2606 OID 24737)
-- Dependencies: 1509 1809 1512
-- Name: nodes_graph_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY nodes
    ADD CONSTRAINT nodes_graph_id_fk FOREIGN KEY (graph_id) REFERENCES graphs(graph_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1819 (class 2606 OID 16626)
-- Dependencies: 1509 1509 1805
-- Name: nodes_type_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY nodes
    ADD CONSTRAINT nodes_type_id_fk FOREIGN KEY (type_id) REFERENCES nodes(node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1822 (class 2606 OID 24727)
-- Dependencies: 1510 1811 1514
-- Name: positions_layout_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY positions
    ADD CONSTRAINT positions_layout_id_fk FOREIGN KEY (layout_id) REFERENCES layouts(layout_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1821 (class 2606 OID 16631)
-- Dependencies: 1509 1805 1510
-- Name: positions_node_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: aurel.paulovic
--

ALTER TABLE ONLY positions
    ADD CONSTRAINT positions_node_id_fk FOREIGN KEY (node_id) REFERENCES nodes(node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1836 (class 0 OID 0)
-- Dependencies: 6
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO "aurel";
GRANT ALL ON SCHEMA public TO "aurel.paulovic";


--
-- TOC entry 1837 (class 0 OID 0)
-- Dependencies: 1508
-- Name: edges; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE edges FROM PUBLIC;
REVOKE ALL ON TABLE edges FROM "aurel.paulovic";
GRANT ALL ON TABLE edges TO "aurel.paulovic";
GRANT ALL ON TABLE edges TO "aurel";


--
-- TOC entry 1839 (class 0 OID 0)
-- Dependencies: 1507
-- Name: element_id_seq; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON SEQUENCE element_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE element_id_seq FROM "aurel.paulovic";
GRANT SELECT,UPDATE ON SEQUENCE element_id_seq TO "aurel.paulovic";
GRANT SELECT,UPDATE ON SEQUENCE element_id_seq TO "aurel";


--
-- TOC entry 1841 (class 0 OID 0)
-- Dependencies: 1511
-- Name: graph_id_seq; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON SEQUENCE graph_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE graph_id_seq FROM "aurel.paulovic";
GRANT ALL ON SEQUENCE graph_id_seq TO "aurel.paulovic";
GRANT SELECT,UPDATE ON SEQUENCE graph_id_seq TO "aurel";


--
-- TOC entry 1842 (class 0 OID 0)
-- Dependencies: 1512
-- Name: graphs; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE graphs FROM PUBLIC;
REVOKE ALL ON TABLE graphs FROM "aurel.paulovic";
GRANT ALL ON TABLE graphs TO "aurel.paulovic";
GRANT ALL ON TABLE graphs TO "aurel";


--
-- TOC entry 1844 (class 0 OID 0)
-- Dependencies: 1513
-- Name: layout_id_seq; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON SEQUENCE layout_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE layout_id_seq FROM "aurel.paulovic";
GRANT ALL ON SEQUENCE layout_id_seq TO "aurel.paulovic";
GRANT SELECT,UPDATE ON SEQUENCE layout_id_seq TO "aurel";


--
-- TOC entry 1845 (class 0 OID 0)
-- Dependencies: 1514
-- Name: layouts; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE layouts FROM PUBLIC;
REVOKE ALL ON TABLE layouts FROM "aurel.paulovic";
GRANT ALL ON TABLE layouts TO "aurel.paulovic";
GRANT ALL ON TABLE layouts TO "aurel";


--
-- TOC entry 1846 (class 0 OID 0)
-- Dependencies: 1515
-- Name: metatypes; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE metatypes FROM PUBLIC;
REVOKE ALL ON TABLE metatypes FROM "aurel.paulovic";
GRANT ALL ON TABLE metatypes TO "aurel.paulovic";
GRANT ALL ON TABLE metatypes TO "aurel";


--
-- TOC entry 1847 (class 0 OID 0)
-- Dependencies: 1509
-- Name: nodes; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE nodes FROM PUBLIC;
REVOKE ALL ON TABLE nodes FROM "aurel.paulovic";
GRANT ALL ON TABLE nodes TO "aurel.paulovic";
GRANT ALL ON TABLE nodes TO "aurel";


--
-- TOC entry 1848 (class 0 OID 0)
-- Dependencies: 1510
-- Name: positions; Type: ACL; Schema: public; Owner: aurel.paulovic
--

REVOKE ALL ON TABLE positions FROM PUBLIC;
REVOKE ALL ON TABLE positions FROM "aurel.paulovic";
GRANT ALL ON TABLE positions TO "aurel.paulovic";
GRANT ALL ON TABLE positions TO "aurel";


-- Completed on 2010-03-14 21:31:32

--
-- PostgreSQL database dump complete
--

