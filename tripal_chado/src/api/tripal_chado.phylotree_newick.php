<?php

/**
 * This code parses the grammer for the Newick format as per the following
 * grammar:
 *
 *  Tree --> Subtree ";" | Branch ";"
 *  Subtree --> Leaf | Internal
 *  Leaf --> Name
 *  Internal --> "(" BranchSet ")" Name
 *  BranchSet --> Branch | BranchSet "," Branch
 *  Branch --> Subtree Length
 *  Name --> empty | string
 *  Length --> empty | ":" number
 *
 */

/**
 *
 * @param unknown $file_name
 */
function tripal_phylogeny_parse_newick_file($file_name) {

  // Initialize the bootstrap value and index
  global $tripal_phylogeny_bootstrap;

  $tripal_phylogeny_bootstrap = 1;
  $tripal_phylogeny_index = 1;

  $tree = [];

  $fp = fopen($file_name, 'r');
  if ($fp) {
    $tree = tripal_phylogeny_parse_newick_tree($fp);
  }
  else {
    // ERROR
  }
  return $tree;
}

/**
 *
 * @param unknown $fp
 * @param number $depth
 *
 * @return boolean
 */
function tripal_phylogeny_parse_newick_tree($fp, $depth = 0) {

  $subtree = tripal_phylogeny_parse_newick_subtree($fp, $depth);
  $subtree['is_root'] = 1;

  // this subtree may also be a branch. A branch is a subtree with a length,
  // so see if there is a length
  $token = tripal_phylogeny_parse_newick_get_token($fp);
  if ($token == ";") {
    // we're done!
    return $subtree;
  }
  tripal_phylogeny_parse_newick_replace_token($fp);

  // Get the length.
  $length = tripal_phylogeny_parse_newick_length($fp, $depth);
  $subtree['length'] = $length;

  // Now if we're missing the semicolon we have a syntax error.
  $token = tripal_phylogeny_parse_newick_get_token($fp);
  if ($token != ';') {
    print "Syntax Error: missing trailing semicolon.\n";
    exit;
  }

  return $subtree;
}

/**
 *
 * @param unknown $fp
 * @param unknown $depth
 *
 * @return Ambigous|unknown
 */
function tripal_phylogeny_parse_newick_subtree($fp, $depth) {

  $internal = tripal_phylogeny_parse_newick_internal($fp, $depth + 1);
  if (!is_array($internal)) {
    $leaf_node = tripal_phylogeny_parse_newick_leaf($fp, $depth);
    return [
      'name' => $leaf_node,
      'depth' => $depth,
      'is_leaf' => TRUE,
      'descendents' => 0,
    ];
  }
  else {
    $internal['depth'] = $depth;
  }
  return $internal;
}

/**
 *
 * @param unknown $fp
 * @param unknown $depth
 *
 * @return boolean|multitype:unknown Ambigous <Ambigous, unknown>
 */
function tripal_phylogeny_parse_newick_branch($fp, $depth) {

  $subtree = tripal_phylogeny_parse_newick_subtree($fp, $depth);
  $length = tripal_phylogeny_parse_newick_length($fp, $depth);

  $subtree['length'] = $length;
  return $subtree;
}

/**
 *
 * @param unknown $fp
 * @param unknown $parent
 * @param unknown $depth
 */
function tripal_phylogeny_parse_newick_internal($fp, $depth) {

  // If the next character is not an open paren then this is an internal node
  if (tripal_phylogeny_parse_newick_get_token($fp) != '(') {
    tripal_phylogeny_parse_newick_replace_token($fp);
    return FALSE;
  }

  $branches = tripal_phylogeny_parse_newick_branchset($fp, $depth);
  if (!is_array($branches)) {
    return FALSE;
  }
  // If we don't have a closing parent then this is a syntax error.
  if (tripal_phylogeny_parse_newick_get_token($fp) != ')') {
    tripal_phylogeny_parse_newick_replace_token($fp);
    return FALSE;
  }
  $internal_node = tripal_phylogeny_parse_newick_name($fp, $depth);
  $descendent_count = 0;
  for ($i = 0; $i < count($branches); $i++) {
    $branches[$i]['parent'] = $internal_node;
    $descendent_count += 1 + $branches[$i]['descendents'];
  }

  return [
    'name' => $internal_node,
    'depth' => $depth,
    'branch_set' => $branches,
    'is_internal' => TRUE,
    'descendents' => $descendent_count,
  ];
}

/**
 *
 * @param unknown $fp
 * @param unknown $parent
 * @param unknown $depth
 */
function tripal_phylogeny_parse_newick_branchset($fp, $depth) {
  $branches = [];

  $num_read = 0;
  $branch = tripal_phylogeny_parse_newick_branch($fp, $depth);
  $branches[] = $branch;

  // If it's not a branch then return false, a branchset will
  // always appear as a branch.
  if (!is_array($branch)) {
    return FALSE;
  }

  // If we have a comma as the next token then this is
  // a branchset and we should recurse.
  $token = tripal_phylogeny_parse_newick_get_token($fp);
  if ($token == ',') {
    $rbranches = tripal_phylogeny_parse_newick_branchset($fp, $depth);
    foreach ($rbranches as $branch) {
      $branches[] = $branch;
    }
  }
  else {
    tripal_phylogeny_parse_newick_replace_token($fp);
  }

  return $branches;
}

/**
 *
 * @param unknown $fp
 * @param unknown $depth
 *
 * @return Ambigous <string, boolean, unknown>
 */
function tripal_phylogeny_parse_newick_leaf($fp, $depth) {
  return tripal_phylogeny_parse_newick_name($fp, $depth);
}

/**
 *
 * @param unknown $fp
 * @param unknown $depth
 *
 * @return string|boolean|Ambigous <string, unknown>
 */
function tripal_phylogeny_parse_newick_name($fp, $depth) {
  global $tripal_phylogeny_bootstrap;

  $token = tripal_phylogeny_parse_newick_get_token($fp);

  // If the next token is a colon, semicolon, close paren, or comma
  // then the name is empty.
  if ($token == ':' or $token == ',' or $token == ';' or $token == ')') {
    tripal_phylogeny_parse_newick_replace_token($fp);
    // create a bootstrap value
    return $tripal_phylogeny_bootstrap++;
  }

  // If the next token is an open paren then this is a syntax error:
  if ($token == '(') {
    tripal_phylogeny_parse_newick_replace_token($fp);
    return FALSE;
  }
  return $token;
}

/**
 *
 * @param unknown $fp
 * @param unknown $depth
 *
 * @return string|boolean|unknown
 */
function tripal_phylogeny_parse_newick_length($fp, $depth) {
  $length = '';

  $token = tripal_phylogeny_parse_newick_get_token($fp);

  // If the next token is a semicolon, close paren, or comma
  // then the length is empty.
  if ($token == ',' or $token == ';' or $token == ')') {
    tripal_phylogeny_parse_newick_replace_token($fp);
    return '';
  }

  // If the next token is a colon then we are parsing the length.
  // Otherwise we are not.
  if ($token != ':') {
    tripal_phylogeny_parse_newick_replace_token($fp);
    return FALSE;
  }

  // Now get the length.
  $token = tripal_phylogeny_parse_newick_get_token($fp);

  // If the next token is an open paren then this is a syntax error:
  if ($token == '(') {
    exit();
  }

  return $token;
}

/**
 *
 * @param unknown $fp
 *
 * @return string
 */
function tripal_phylogeny_parse_newick_get_token($fp) {

  // Keep track of the file position that we start with
  global $tripal_phylogeny_fp_pos;
  $tripal_phylogeny_fp_pos = ftell($fp);

  $token = '';
  $in_quote = FALSE;
  $num_read = 0;

  $c = fgetc($fp);
  while (!feof($fp)) {
    $num_read++;

    switch ($c) {
      // If the first character is a reserved character and we
      // we have not encountered any other charcters then return
      // it as the token. Otherwise, return the collected token.
      case ';':
      case '(':
      case ')':
      case ',':
      case ':':
        if (!$token) {
          return $c;
        }
        else {
          // put the character back and return the token
          fseek($fp, $tripal_phylogeny_fp_pos + $num_read - 1);
          return $token;
        }

        break;
      // Quotes are allowed around names and if a name is in
      // quotes then allow spaces. Otherwise, spaces are ignored.
      case '\'':
      case '"':
        if (!$in_quote) {
          $in_quote = TRUE;
        }
        else {
          $in_quote = FALSE;
        }
        break;
      case " ":
      case "\t":
      case "\r":
      case "\n":
        if ($in_quote) {
          $token .= $c;
        }
        break;
      // All other characters get saved as the token
      default:
        $token .= $c;
    }
    $c = fgetc($fp);
  }
  return $token;
}

/**
 *
 * @param unknown $fp
 */
function tripal_phylogeny_parse_newick_replace_token($fp) {
  global $tripal_phylogeny_fp_pos;

  fseek($fp, $tripal_phylogeny_fp_pos);
  $tripal_phylogeny_fp_pos = ftell($fp);
}