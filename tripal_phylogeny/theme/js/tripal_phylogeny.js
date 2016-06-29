/* phylotree d3js graphs */

(function ($) {
  
  var height = 0; // will be dynamically sized
  
  $(document).ready( function () {
        
    var nodeSize = function(d) {
      var size;
      if (d.cvterm_name == "phylo_root") {
        size = treeOptions['root_node_size']; 
      }
      if (d.cvterm_name == "phylo_interior") {
        size = treeOptions['interior_node_size']; 
      }
      if (d.cvterm_name == "phylo_leaf") {
        size = treeOptions['leaf_node_size']; 
      }
        return size;
    }

    // function to generate color based on the organism genus and species
    // on graph node d
    var organismColor = function(d) {
      var color = null;
      if (d.fo_genus) {
        color = organismColors[d.fo_genus + ' ' + d.fo_species];
      }
      if (color) { return color; }
      else { return 'grey'; }
    };

    // callback for mouseover event on graph node d
    var nodeMouseOver = function(d) {
      var el =$(this);
      el.attr('cursor', 'pointer');
      var circle = el.find('circle');
      // highlight in yellow no matter if leaf or interior node
      circle.attr('fill', 'yellow');
      if(! d.children) {
        // only leaf nodes have descriptive text
        var txt = el.find('text');
        txt.attr('font-weight', 'bold');
      }
    };
    
    // callback for mouseout event on graph node d
    var nodeMouseOut = function(d) {
      var el = $(this);
      el.attr('cursor', 'default');
      var circle = el.find('circle');
      if(! d.children) {
        // restore the color based on organism id for leaf nodes
        circle.attr('fill', organismColor(d));
        var txt = el.find('text');
        txt.attr('font-weight', 'normal');
      }
      else {
        // restore interior nodes to white
        circle.attr('fill', 'white');
      }
    };
    
    // callback for mousedown/click event on graph node d
    var nodeMouseDown = function(d) {
      var el = $(this);
      var dialog = $('#phylonode_popup_dialog');
      var title = (! d.children ) ? d.name : 'interior node ' + d.phylonode_id;

      if(d.children) {
        // interior node
        if(d.phylonode_id) {
          var link = $('#phylonode_context_link');
          // eventually, this link will be replaced with something internal to the site; note that the trailing slash is somewhat important to avoid apparent hanging due to the way django handles url pattern matching
          link.attr('href', 'http://comparative-legumes.org/chado/context_viewer/' + d.phylonode_id + '/');
          link.text('View Genomic Contexts for genes in this subtree');
          link.show();
        }
        else {
          // this shouldn't happen but ok
          $('#phylonode_context_link').hide();
        }
        
        // show dialog content relevant for interior node
        // go_link not ready for prime time
        // $('#phylonode_go_link').show();
        $('#phylonode_go_link').hide();
        $('#phylonode_context_link').show();
        
        // hide dialog content which is only applicable to leaf nodes
        $('#phylonode_organism_link').hide();
        $('#phylonode_feature_link').hide();
      }
      else {
        // leaf node

        // show dialog content relevant for leaf node
        $('#phylonode_organism_link').show();
        $('#phylonode_feature_link').show();
        
        // hide dialog content which is only applicable to interior nodes
        $('#phylonode_go_link').hide();
        $('#phylonode_context_link').hide();
        
        if(d.feature_node_id) {
          var link = $('#phylonode_feature_link');
          link.attr('href', '?q=node/' + d.feature_node_id);
          link.text('view feature: ' + d.feature_name);
          link.show();

        }
        else {
          // this shouldn't happen but ok
          $('#phylonode_feature_link').hide();
        }
        if (d.feature_name) {
          var link = $('#phylonode_gene_linkout');
          //FIXME: hack depending on typical naming conventions. we can certainly do better
          var transcript = d.feature_name.replace(/^.....\./, "");
          var gene = transcript.replace(/\.\d+$/, "");
          if (d.fo_genus == 'Glycine' && d.fo_species == 'max') {
                  link.attr('href', 'http://www.soybase.org/sbt/search/search_results.php?category=FeatureName&search_term=' + gene);
                  //link.attr('href', 'http://www.soybase.org/gb2/gbrowse/gmax2.0/?q=' + gene + ';dbid=gene_models_wm82_a2_v1');
                  link.text('view at SoyBase: ' + gene);
                  link.show();
          }
          else if (d.fo_genus == 'Phaseolus' && d.fo_species == 'vulgaris') {
                  link.attr('href', 'http://phytozome.jgi.doe.gov/pz/portal.html#!gene?organism=Pvulgaris&searchText=locusName:' + gene);
                  link.text('view at Phytozome: ' + gene);
                  link.show();
          }
          else if (d.fo_genus == 'Medicago' && d.fo_species == 'truncatula') {
                  link.attr('href', 'http://medicago.jcvi.org/medicago/jbrowse/?data=data%2Fjson%2Fmedicago&loc='+transcript+'&tracks=DNA%2Cgene_models&highlight=');
                  link.text('view at JCVI: ' + transcript);
                  link.show();
          }
          else if (d.fo_genus == 'Arachis' && d.fo_species == 'duranensis') {
                  //link.attr('href', 'http://peanutbase.org/gb2/gbrowse/Aradu1.0/?q=' + gene);
                  //link.attr('href', 'http://peanutbase-stage.agron.iastate.edu/gb2/gbrowse/Aradu1.0/?q=' + gene);
                  link.attr('href', 'http://peanutbase.org/gb2/gbrowse/Aradu1.0/?q=' + gene + ';dbid=gene_models');
                  //link.attr('href', 'http://peanutbase-stage.agron.iastate.edu/gb2/gbrowse/Aradu1.0/?q=' + gene + ';dbid=gene_models');
                  link.text('view at PeanutBase: ' + gene);
                  link.show();
          }
          else if (d.fo_genus == 'Arachis' && d.fo_species == 'ipaensis') {
                  //link.attr('href', 'http://peanutbase.org/gb2/gbrowse/Araip1.0/?q=' + gene);
                  //link.attr('href', 'http://peanutbase-stage.agron.iastate.edu/gb2/gbrowse/Araip1.0/?q=' + gene);
                  link.attr('href', 'http://peanutbase.org/gb2/gbrowse/Araip1.0/?q=' + gene + ';dbid=gene_models');
                  //link.attr('href', 'http://peanutbase-stage.agron.iastate.edu/gb2/gbrowse/Araip1.0/?q=' + gene + ';dbid=gene_models');
                  link.text('view at PeanutBase: ' + gene);
                  link.show();
          }
          else if (d.fo_genus == 'Arabidopsis' && d.fo_species == 'thaliana') {
                  link.attr('href', ' http://www.araport.org/locus/' + gene);
                  link.text('view at Arabidopsis Information Portal: ' + gene);
                  link.show();
          }
          else {
                  link.hide();
          }
        
        }
        if(d.fo_organism_id) {
          var link = $('#phylonode_organism_link');
          link.attr('href', d.fo_organism_node_id ?
                    '?q=node/' + d.fo_organism_node_id : '#');
          link.text('view organism: ' + d.fo_genus + ' '+
                    d.fo_species + ( d.fo_common_name ? ' (' + d.fo_common_name + ')' : '' ) );
          link.show();
        } 
        else {
          // there should always be an organism id, but degrade gracefully
          $('#phylonode_organism_link').hide();
        }
      }
    
      dialog.dialog( {
        title : title,
        closeOnEscape : false,
        modal : false,
        position : { my : 'center center', at : 'center center', of : el },
        show : { effect: 'blind', direction: 'down', duration: 200 }
      });
    };

    $.getJSON(phylotreeDataURL, function(treeData) {
      displayData(treeData);
      $('.phylogram-ajax-loader').remove();
    });

    function displayData(treeData) {
      height = graphHeight(treeData);
      d3.phylogram.build('#phylogram', treeData, {
        'width' : treeOptions['phylogram_width'],
        'height' : height,
        'fill' : organismColor,
        'size' : nodeSize,
        'nodeMouseOver' : nodeMouseOver,
        'nodeMouseOut' : nodeMouseOut,
        'nodeMouseDown' : nodeMouseDown
      });
      d3.phylogram.buildRadial('#phylotree-radial-graph', treeData, {
        'width' : treeOptions['dendrogram_width'], // square graph 
        'fill' : organismColor,
        'size' : nodeSize,
        'nodeMouseOver' : nodeMouseOver,
        'nodeMouseOut' : nodeMouseOut,
        'nodeMouseDown' : nodeMouseDown
      });
      organismBubblePlot('#phylotree-organisms', treeData, {
        'height' : treeOptions['bubble_width'], // square graph
        'width' : treeOptions['bubble_width'], 
        'fill' : organismColor,
        'nodeMouseOver' : nodeMouseOver,
        'nodeMouseOut' : nodeMouseOut,
        'nodeMouseDown' : nodeMouseDown
      });
    }

    /* graphHeight() generate graph height based on leaf nodes */
    function graphHeight(data) {
      function countLeafNodes(node) {
        if(! node.children) {
          return 1;
        }
        var ct = 0;
        node.children.forEach( function(child) {
          ct+= countLeafNodes(child);
        });
        return ct;
      }
      var leafNodeCt = countLeafNodes(data);
      return 22 * leafNodeCt;
    }
  });
})(jQuery);
