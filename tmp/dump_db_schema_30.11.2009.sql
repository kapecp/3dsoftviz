--
-- PostgreSQL database dump
--

-- Started on 2009-11-30 22:31:22

SET client_encoding = 'UTF8';

SET search_path = public, pg_catalog;

ALTER TABLE ONLY public.positions DROP CONSTRAINT positions_node_id_fk;
ALTER TABLE ONLY public.nodes DROP CONSTRAINT nodes_type_id_fk;
ALTER TABLE ONLY public.edges DROP CONSTRAINT edges_type_id_fk;
ALTER TABLE ONLY public.edges DROP CONSTRAINT edges_n2_fk;
ALTER TABLE ONLY public.edges DROP CONSTRAINT edges_n1_fk;
DROP INDEX public.fki_nodes_type_id_fk;
DROP INDEX public.fki_edges_type_id_fk;
DROP INDEX public.fki_edges_n2_fk;
DROP INDEX public.fki_edges_n1_fk;
ALTER TABLE ONLY public.positions DROP CONSTRAINT positions_layout_node_pk;
ALTER TABLE ONLY public.nodes DROP CONSTRAINT ndoes_node_id_pk;
ALTER TABLE ONLY public.edges DROP CONSTRAINT edges_edge_id_pk;
DROP TABLE public.positions;
DROP TABLE public.nodes;
DROP TABLE public.edges;
DROP SEQUENCE public.element_id_seq;
DROP SCHEMA public;
--
-- TOC entry 3 (class 2615 OID 2200)
-- Name: public; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA public;


--
-- TOC entry 1762 (class 0 OID 0)
-- Dependencies: 3
-- Name: SCHEMA public; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA public IS 'standard public schema';


SET search_path = public, pg_catalog;

--
-- TOC entry 1471 (class 1259 OID 27476)
-- Dependencies: 3
-- Name: element_id_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE element_id_seq
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 1472 (class 1259 OID 27486)
-- Dependencies: 1742 1743 3
-- Name: edges; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE edges (
    edge_id bigint DEFAULT nextval('element_id_seq'::regclass) NOT NULL,
    name character varying,
    type_id bigint NOT NULL,
    n1 bigint NOT NULL,
    n2 bigint NOT NULL,
    oriented boolean DEFAULT true NOT NULL
);


--
-- TOC entry 1470 (class 1259 OID 27468)
-- Dependencies: 1740 1741 3
-- Name: nodes; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE nodes (
    node_id bigint DEFAULT nextval('element_id_seq'::regclass) NOT NULL,
    name character varying,
    type_id bigint DEFAULT currval('element_id_seq'::regclass) NOT NULL
);


--
-- TOC entry 1473 (class 1259 OID 27701)
-- Dependencies: 1744 3
-- Name: positions; Type: TABLE; Schema: public; Owner: -; Tablespace: 
--

CREATE TABLE positions (
    layout_id integer DEFAULT 0 NOT NULL,
    node_id bigint NOT NULL,
    pos_x double precision NOT NULL,
    pos_y double precision NOT NULL,
    pos_z double precision NOT NULL
);


--
-- TOC entry 1749 (class 2606 OID 27493)
-- Dependencies: 1472 1472
-- Name: edges_edge_id_pk; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_edge_id_pk PRIMARY KEY (edge_id);


--
-- TOC entry 1747 (class 2606 OID 27475)
-- Dependencies: 1470 1470
-- Name: ndoes_node_id_pk; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY nodes
    ADD CONSTRAINT ndoes_node_id_pk PRIMARY KEY (node_id);


--
-- TOC entry 1754 (class 2606 OID 27706)
-- Dependencies: 1473 1473 1473
-- Name: positions_layout_node_pk; Type: CONSTRAINT; Schema: public; Owner: -; Tablespace: 
--

ALTER TABLE ONLY positions
    ADD CONSTRAINT positions_layout_node_pk PRIMARY KEY (layout_id, node_id);


--
-- TOC entry 1750 (class 1259 OID 27510)
-- Dependencies: 1472
-- Name: fki_edges_n1_fk; Type: INDEX; Schema: public; Owner: -; Tablespace: 
--

CREATE INDEX fki_edges_n1_fk ON edges USING btree (n1);


--
-- TOC entry 1751 (class 1259 OID 27511)
-- Dependencies: 1472
-- Name: fki_edges_n2_fk; Type: INDEX; Schema: public; Owner: -; Tablespace: 
--

CREATE INDEX fki_edges_n2_fk ON edges USING btree (n2);


--
-- TOC entry 1752 (class 1259 OID 27512)
-- Dependencies: 1472
-- Name: fki_edges_type_id_fk; Type: INDEX; Schema: public; Owner: -; Tablespace: 
--

CREATE INDEX fki_edges_type_id_fk ON edges USING btree (type_id);


--
-- TOC entry 1745 (class 1259 OID 27484)
-- Dependencies: 1470
-- Name: fki_nodes_type_id_fk; Type: INDEX; Schema: public; Owner: -; Tablespace: 
--

CREATE INDEX fki_nodes_type_id_fk ON nodes USING btree (type_id);


--
-- TOC entry 1756 (class 2606 OID 27685)
-- Dependencies: 1470 1746 1472
-- Name: edges_n1_fk; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_n1_fk FOREIGN KEY (n1) REFERENCES nodes(node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1757 (class 2606 OID 27690)
-- Dependencies: 1470 1472 1746
-- Name: edges_n2_fk; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_n2_fk FOREIGN KEY (n2) REFERENCES nodes(node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1758 (class 2606 OID 27695)
-- Dependencies: 1470 1746 1472
-- Name: edges_type_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY edges
    ADD CONSTRAINT edges_type_id_fk FOREIGN KEY (type_id) REFERENCES nodes(node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1755 (class 2606 OID 27717)
-- Dependencies: 1470 1470 1746
-- Name: nodes_type_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY nodes
    ADD CONSTRAINT nodes_type_id_fk FOREIGN KEY (type_id) REFERENCES nodes(node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1759 (class 2606 OID 27707)
-- Dependencies: 1473 1746 1470
-- Name: positions_node_id_fk; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY positions
    ADD CONSTRAINT positions_node_id_fk FOREIGN KEY (node_id) REFERENCES nodes(node_id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 1763 (class 0 OID 0)
-- Dependencies: 3
-- Name: public; Type: ACL; Schema: -; Owner: -
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- TOC entry 1764 (class 0 OID 0)
-- Dependencies: 1471
-- Name: element_id_seq; Type: ACL; Schema: public; Owner: -
--

REVOKE ALL ON SEQUENCE element_id_seq FROM PUBLIC;
REVOKE ALL ON SEQUENCE element_id_seq FROM postgres;
GRANT ALL ON SEQUENCE element_id_seq TO postgres;
GRANT ALL ON SEQUENCE element_id_seq TO PUBLIC;


--
-- TOC entry 1765 (class 0 OID 0)
-- Dependencies: 1472
-- Name: edges; Type: ACL; Schema: public; Owner: -
--

REVOKE ALL ON TABLE edges FROM PUBLIC;
REVOKE ALL ON TABLE edges FROM postgres;
GRANT ALL ON TABLE edges TO postgres;
GRANT ALL ON TABLE edges TO PUBLIC;


--
-- TOC entry 1766 (class 0 OID 0)
-- Dependencies: 1470
-- Name: nodes; Type: ACL; Schema: public; Owner: -
--

REVOKE ALL ON TABLE nodes FROM PUBLIC;
REVOKE ALL ON TABLE nodes FROM postgres;
GRANT ALL ON TABLE nodes TO postgres;
GRANT ALL ON TABLE nodes TO PUBLIC;


--
-- TOC entry 1767 (class 0 OID 0)
-- Dependencies: 1473
-- Name: positions; Type: ACL; Schema: public; Owner: -
--

REVOKE ALL ON TABLE positions FROM PUBLIC;
REVOKE ALL ON TABLE positions FROM postgres;
GRANT ALL ON TABLE positions TO postgres;
GRANT ALL ON TABLE positions TO PUBLIC;


-- Completed on 2009-11-30 22:31:22

--
-- PostgreSQL database dump complete
--

