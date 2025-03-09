<?php


namespace Drupal\tripal_chado\Services;


use Drupal\Core\Database\Connection;
use Drupal\Core\Messenger\MessengerInterface;
use Drupal\Core\Messenger\MessengerTrait;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\Core\StringTranslation\TranslationInterface;
use Drupal\tripal_chado\Database\ChadoConnection;


/**
 * Repository for database-related helper methods for our example.
 *
 * This repository is a service named 'dbtng_example.repository'. You can see
 * how the service is defined in dbtng_example/dbtng_example.services.yml.
 *
 * For projects where there are many specialized queries, it can be useful to
 * group them into 'repositories' of queries. We can also architect this
 * repository to be a service, so that it gathers the database connections it
 * needs. This way other classes which use the repository don't need to concern
 * themselves with database connections, only with business logic.
 *
 * This repository demonstrates basic CRUD behaviors, and also has an advanced
 * query which performs a join with the user table.
 *
 * @ingroup dbtng_example
 */
class GenomicFeatureHelper {

    use MessengerTrait;
    use StringTranslationTrait;

    /**
     * The database connection.
     *
     * @var \Drupal\Core\Database\Connection
     */
    protected $connection;

    /**
     * Construct a repository object.
     *
     * @param \Drupal\Core\Database\Connection $connection
     *   The database connection.
     * @param \Drupal\Core\StringTranslation\TranslationInterface $translation
     *   The translation service.
     * @param \Drupal\Core\Messenger\MessengerInterface $messenger
     *   The messenger service.
     */
    public function __construct(Connection $connection, TranslationInterface $translation, MessengerInterface $messenger) {
        $this->connection = $connection;
        $this->setStringTranslation($translation);
        $this->setMessenger($messenger);
    }


    /**
     * Read from the database using a filter array.
     *
     * The standard function to perform reads for static queries is
     * Connection::query().
     *
     * Connection::query() uses an SQL query with placeholders and arguments as
     * parameters.
     *
     * Drupal DBTNG provides an abstracted interface that will work with a wide
     * variety of database engines.
     *
     * The following is a query which uses a string literal SQL query. The
     * placeholders will be substituted with the values in the array. Placeholders
     * are marked with a colon ':'. Table names are marked with braces, so that
     * Drupal's' multisite feature can add prefixes as needed.
     *
     * @code
     *   // SELECT * FROM {dbtng_example} WHERE uid = 0 AND name = 'John'
     *   \Drupal::database()->query(
     *     "SELECT * FROM {dbtng_example} WHERE uid = :uid and name = :name",
     *     [':uid' => 0, ':name' => 'John']
     *   )->execute();
     * @endcode
     *
     * For more dynamic queries, Drupal provides Connection::select() API method,
     * so there are several ways to perform the same SQL query. See the
     * @link http://drupal.org/node/310075 handbook page on dynamic queries. @endlink
     * @code
     *   // SELECT * FROM {dbtng_example} WHERE uid = 0 AND name = 'John'
     *   \Drupal::database()->select('dbtng_example')
     *     ->fields('dbtng_example')
     *     ->condition('uid', 0)
     *     ->condition('name', 'John')
     *     ->execute();
     * @endcode
     *
     * Here is select() with named placeholders:
     * @code
     *   // SELECT * FROM {dbtng_example} WHERE uid = 0 AND name = 'John'
     *   $arguments = array(':name' => 'John', ':uid' => 0);
    *   \Drupal::database()->select('dbtng_example')
     *     ->fields('dbtng_example')
     *     ->where('uid = :uid AND name = :name', $arguments)
     *     ->execute();
     * @endcode
     *
     * Conditions are stacked and evaluated as AND and OR depending on the type of
     * query. For more information, read the conditional queries handbook page at:
     * http://drupal.org/node/310086
     *
     * The condition argument is an 'equal' evaluation by default, but this can be
     * altered:
     * @code
     *   // SELECT * FROM {dbtng_example} WHERE age > 18
     *   \Drupal::database()->select('dbtng_example')
     *     ->fields('dbtng_example')
     *     ->condition('age', 18, '>')
     *     ->execute();
     * @endcode
     *
     * @param array $entry
     *   An array containing all the fields used to search the entries in the
     *   table.
     *
     * @return object
     *   An object containing the loaded entries if found.
     *
     * @see Drupal\Core\Database\Connection::select()
     */
    public function load(array $entry = [], array $header = []) {
        // Read all the fields from the dbtng_example table.
        $select = $this->connection
            ->select('chado.feature', 'f')->extend('Drupal\Core\Database\Query\TableSortExtender')
            ->extend('Drupal\Core\Database\Query\PagerSelectExtender')->limit(10);

        $select->join('chado.f_type', 't', 'f.type_id = t.type_id');

        $select->addField('f', 'name');
        $select->addField('f', 'uniquename');
        $select->addField('t', 'type', 'type');
        $select->addField('f', 'feature_id');

        // Add each field and value as a condition to this query.
        $operator = '='; // default operator
        foreach ($entry as $field => $value) {
            if (is_array($value)) {
                $operator = $value['operator'];
                $value = $value['value'];
            }
            if ($value == '') {
                continue; ## don't add a filter for empty values
            }
            if ($operator == 'contains') {
                $value = "%$value%";
                $operator = 'LIKE';
            }
            $select->condition($field, $value, $operator);
        }
        $select->orderByHeader($header);
        // Return the result in object format.
        return $select->distinct()->execute()->fetchAll();
    }

    public function getFeatureInfoById(int $id) {

        $select = $this->connection
            ->select('chado.feature', 'f');
        $select->join('chado.f_type', 't', 'f.type_id = t.type_id');
        $select->join('chado.organism', 'o', 'f.organism_id = o.organism_id');
        $select->addField('f', 'name');
        $select->addField('f', 'residues');

        $select->addField('f', 'uniquename');
        $select->addField('t', 'type', 'type');

        $select->addField('f', 'feature_id');
        $select->addField('o', 'genus');
        $select->addField('o', 'species');
        $select->addField('o', 'organism_id');
        $select->condition('f.feature_id', $id);
        return $select->execute()->fetch();
    }

    public function getGeneticCodeByOrganism(int $id, bool $mito = FALSE) {
        $sql= "SELECT abbreviation, op.value, cvt.name FROM chado.organism o
            INNER JOIN chado.organismprop op ON o.organism_id = op.organism_id
            INNER JOIN chado.cvterm cvt ON op.type_id = cvt.cvterm_id
            WHERE cvt.name = :code AND op.organism_id = :id";

        $query = \Drupal::database()->query($sql, ['code' => ($mito) ? 'mitochondrial_genetic_code' : 'genetic_code' , 'id' => $id]);
        return($query->fetch()->value);

    }



    /*
     * Table taken from: https://www.biophp.org/minitools/sequence_manipulation_and_data/
     * But allows to preserve lower case
     */

    function Complement($seq): string{

        $comptable =  [
            'A' => 'T',
            'T' => 'A',
            'G' => 'C',
            'C' => 'G',
            'Y' => 'R',
            'W' => 'W',
            'K' => 'M',
            'M' => 'K',
            'D' => 'H',
            'V' => 'B',
            'H' => 'D',
            'B' => 'V',
        ];
        foreach ($comptable as $key => $value) {
	        $comptable[strtolower($key)] = strtolower($value);
        }

        $seq = strtr($seq, $comptable);

        return $seq;
    }

    /**
     * Found in BioPHP
     * (https://www.biophp.org/minitools/dna_to_protein/) by joseba
     *
     */

    function translate_DNA_to_protein($seq, $genetic_code): string|null{
        if (empty($seq) or $seq == '') return '';
        // $aminoacids is the array of aminoacids
        $aminoacids=array("F","L","I","M","V","S","P","T","A","Y","*","H","Q","N","K","D","E","C","W","R","G","X");

        // $triplets is the array containning the genetic codes
        // Info has been extracted from http://www.ncbi.nlm.nih.gov/Taxonomy/Utils/wprintgc.cgi?mode

        // Standard genetic code
        $triplets[1]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AGT |AGC )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TAG |TGA )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG )","(CG. |AGA |AGG )","(GG. )","(\S\S\S )");
        // Vertebrate Mitochondrial
        $triplets[2]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AGT |AGC )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TAG |AGA |AGG )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG |TGA )","(CG. )","(GG. )","(\S\S\S )");
        // Yeast Mitochondrial
        $triplets[3]=array("(TTT |TTC )","(TTA |TTG )","(ATT |ATC )","(ATG |ATA )","(GT. )","(TC. |AGT |AGC )",
                        "(CC. )","(AC. |CT. )","(GC. )","(TAT |TAC )","(TAA |TAG )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG |TGA )","(CG. |AGA |AGG )","(GG. )","(\S\S\S )");
        // Mold, Protozoan and Coelenterate Mitochondrial. Mycoplasma, Spiroplasma
        $triplets[4]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AGT |AGC )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TAG )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG |TGA )","(CG. |AGA |AGG )","(GG. )","(\S\S\S )");
        // Invertebrate Mitochondrial
        $triplets[5]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC )","(ATG |ATA )","(GT. )","(TC. |AG. )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TAG )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG |TGA )","(CG. )","(GG. )","(\S\S\S )");
        // Ciliate Nuclear; Dasycladacean Nuclear; Hexamita Nuclear
        $triplets[6]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AGT |AGC )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TGA )","(CAT |CAC )",
                        "(CAA |CAG |TAA |TAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG )","(CG. |AGA |AGG )","(GG. )","(\S\S\S )");
        // Echinoderm Mitochondrial
        $triplets[9]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AG. )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TAG )","(CAT |CAC )",
                        "(CAA |CAG )","(AAA |AAT |AAC )","(AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG |TGA )","(CG. )","(GG. )","(\S\S\S )");
        // Euplotid Nuclear
        $triplets[10]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AGT |AGC )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TAG )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC |TGA )",
                        "(TGG )","(CG. |AGA |AGG )","(GG. )","(\S\S\S )");
        // Bacterial and Plant Plastid
        $triplets[11]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AGT |AGC )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TAG |TGA )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG )","(CG. |AGA |AGG )","(GG. )","(\S\S\S )");
        // Alternative Yeast Nuclear
        $triplets[12]=array("(TTT |TTC )","(TTA |TTG |CTA |CTT |CTC )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AGT |AGC |CTG )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TAG |TGA )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG )","(CG. |AGA |AGG )","(GG. )","(\S\S\S )");
        // Ascidian Mitochondrial
        $triplets[13]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC )","(ATG |ATA )","(GT. )","(TC. |AGT |AGC )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TAG )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG |TGA )","(CG. )","(GG. |AGA |AGG )","(\S\S\S )");
        // Flatworm Mitochondrial
        $triplets[14]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AG. )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC |TAA )","(TAG )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC |AAA )","(AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG |TGA )","(CG. )","(GG. )","(\S\S\S )");
        // Blepharisma Macronuclear
        $triplets[15]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AGT |AGC )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TGA )","(CAT |CAC )",
                        "(CAA |CAG |TAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG )","(CG. |AGA |AGG )","(GG. )","(\S\S\S )");
        // Chlorophycean Mitochondrial
        $triplets[16]=array("(TTT |TTC )","(TTA |TTG |CT. |TAG )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AGT |AGC )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TGA )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG )","(CG. |AGA |AGG )","(GG. )","(\S\S\S )");
        // Trematode Mitochondrial
        $triplets[21]=array("(TTT |TTC )","(TTA |TTG |CT. )","(ATT |ATC )","(ATG |ATA )","(GT. )","(TC. |AG. )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TAG )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC |AAA )","(AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG |TGA )","(CG. )","(GG. )","(\S\S\S )");
        // Scenedesmus obliquus mitochondrial
        $triplets[22]=array("(TTT |TTC )","(TTA |TTG |CT. |TAG )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TCT |TCC |TCG |AGT |AGC )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TAA |TGA |TCA )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG )","(CG. |AGA |AGG )","(GG. )","(\S\S\S )");
        // Thraustochytrium mitochondrial code
        $triplets[23]=array("(TTT |TTC )","(TTG |CT. )","(ATT |ATC |ATA )","(ATG )","(GT. )","(TC. |AGT |AGC )",
                        "(CC. )","(AC. )","(GC. )","(TAT |TAC )","(TTA |TAA |TAG |TGA )","(CAT |CAC )",
                        "(CAA |CAG )","(AAT |AAC )","(AAA |AAG )","(GAT |GAC )","(GAA |GAG )","(TGT |TGC )",
                        "(TGG )","(CG. |AGA |AGG )","(GG. )","(\S\S\S )");
        $seq = strtoupper($seq);
        // place a space after each triplete in the sequence
        $temp = chunk_split($seq,3,' ');

        // replace triplets by corresponding amnoacid
        $peptide = preg_replace (pattern: array($triplets[$genetic_code]), replacement: $aminoacids, subject: $temp);

        // return peptide sequence
        return $peptide;
}





    public function spliceSeq(string $seq, int $fid, int $sid) {

        $select = $this->connection
            ->select('chado.featureloc', 'l');

        $select->fields('l');
        $select->condition('feature_id', $fid);
        $select->condition('srcfeature_id', $sid);
        $ranges = $select->execute()->fetchAll();

        $subseq = '';

        foreach ($ranges as $range) {
            // suseq is 0-based
            $fmin = (int) $range->fmin; $fmax = (int) $range->fmax;

            $substr = substr($seq, $fmin, $fmax - $fmin);

            if ($range->strand < 0){
                $substr = strrev($this->Complement($substr));

            }
            $subseq .= $substr;

        }

        return $subseq;

    }

    /**
     * Retrieve the sequence recursively from the source feature
     *
     **/
    public function getSeqRecursive(int $id) {
	    // get the residues, if any
	error_log("getSeqRecursive: Looking for feature ". $id);
	$select = $this->connection
            ->select('chado.feature', 'f');
        $select->addField('f', 'residues');
        $select->condition('f.feature_id', $id);
        $entry = $select->execute()->fetchAssoc();
	\Drupal::messenger()->addWarning(t('Finished parsing entry..'));
	error_log('finished getting entry');
        $residues = $entry['residues'];
        // Check if this feature has a featureloc entry
        $select = $this->connection
            ->select('chado.featureloc', 'l');
        $select->fields('l');
        $select->condition('l.feature_id', $id)->distinct()
        ->orderBy('l.srcfeature_id');
        $entries =  $select->execute()->fetchAll();
	// if there are no sourcefeatures, we are done and can return the residues
	// some source features could have their own id as source
	//
	error_log("found ".count($entries). " srcfeatures");

	    if ($entry->srcfeature_id == $id) {
		error_log("Feature $id has itself as srcfeature!");
	    }
	    if (empty($residues) or $residues == '' ) {
		error_log("Empty residues for feature $id");
	    }
	    error_log("returning residues: ". $residues);

      if (count($entries) == 0 or $entries[0]->srcfeature_id == $id)  {
        // list is empty.
	    return [$id => $residues];
        }
        // We need to get the sequence of the source feature, hope it has residues
        // If the sequences were imported from GFF, the coordinates should always
        // be given with respect to a landmark,
        // so there shouldn't be the need to look for nested alignments
        // In principle we simply bail out if the nesting is deeper, even though the
        // DB schema allows for it
        $retarray = [];
        // First, check what kind of feature we have
        $select = $this->connection->select('chado.f_type', 'ft')
            ->fields('ft', ['type'])->condition('ft.feature_id', $id);
        $type = $select->execute()->fetch()->type;
	foreach ($entries as $entry) {
		error_log("looking up Srcfeatureid: ".$entry->srcfeature_id);
            if ($entry->srcfeature_id  and $entry->srcfeature_id != $id ) {
                $srcseqs = $this->getSeqRecursive($entry->srcfeature_id);
                // splice the sequences, one by one
                foreach ($srcseqs as $sid => $sseq) {
                    $myseq = $this->spliceSeq($sseq, $id, $sid);
                    // check if we need to translate the sequence
                    // for a standard gff import, checking if this is a polypeptide should be ok
                    // however, there might be cases where this is not sufficient
                    if ($type == 'polypeptide') {
                        // in principle, we need to infer the genetic code from the organism,
                        // however, this information is not stored in the organsim table

                        $gcode = $this->getGeneticCodeByOrganism($this->defaultLookup('feature', $id)->organism_id);
                        $myseq = $this->translate_DNA_to_protein($myseq, $gcode);
                    }
                    $retarray[$sid] = $myseq;
                }
            } else {
                $retarray[$id] = $residues;
            }
	}
	//error_log(var_dump($retarray));
        return $retarray;
    }

    /**
    * Do some postprocessing of the array
    * Replaces the id with the uname, and removes the entry if only residues were retrieved
    */
    public function getSeq(int $id) {
        $seqArr = $this->getSeqRecursive($id);
        $out = [];
        foreach ($seqArr as $sid => $value) {
            if ($id != $sid) {
                $uname = $this->defaultLookup('feature', $sid)->uniquename;


                $out[$uname] = $value;
            }
        }
        return $out;
    }




    public function defaultLookup(string $chadotype, int $id) {

        $select = $this->connection
            ->select('chado.'.$chadotype, 't')
            ->fields('t')
            ->condition('t.'.$chadotype.'_id', $id);
        return $select->execute()->fetch();

    }

    /**
	 * Counts the number of feature entries in Chado DB,
  */

 public function countFeatureTypes() {


     $sql = "SELECT type,count(type)  FROM chado.f_type
         GROUP BY type ORDER BY type;";

     $query = \Drupal::database()->query($sql);
     return($query->fetchAll());

    }


}
