CREATE TABLE IF NOT EXISTS biomaterial_project (
    biomaterial_project_id bigserial primary key NOT NULL,
    biomaterial_id bigint NOT NULL,
    project_id bigint NOT NULL,
    CONSTRAINT biomaterial_project_c1 UNIQUE (biomaterial_id, project_id),
    FOREIGN KEY (biomaterial_id) REFERENCES biomaterial(biomaterial_id) ON DELETE CASCADE,
    FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE
);

CREATE INDEX  biomaterial_project_idx1 ON biomaterial_project USING btree (biomaterial_id);
CREATE INDEX  biomaterial_project_idx2 ON biomaterial_project USING btree (project_id);

COMMENT ON TABLE project_stock IS 'This table is intended associate records in the biomaterial table with a project.';


-- ================================================
-- TABLE: stock_biomaterial
-- ================================================
CREATE TABLE IF NOT EXISTS stock_biomaterial (
    stock_biomaterial_id bigserial primary key NOT NULL,
    biomaterial_id bigint NOT NULL,
    stock_id bigint NOT NULL,
    CONSTRAINT stock_biomaterial_c1 UNIQUE (biomaterial_id, stock_id),
    FOREIGN KEY (biomaterial_id) REFERENCES biomaterial(biomaterial_id) ON DELETE CASCADE,
    FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE
);

CREATE INDEX  stock_biomaterial_idx1 ON stock_biomaterial USING btree (biomaterial_id);
CREATE INDEX  stock_biomaterial_idx2 ON stock_biomaterial USING btree (stock_id);

COMMENT ON TABLE stock_biomaterial IS 'Associates records in the biomaterial table with a stock.';
