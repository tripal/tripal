// Adapted from https://github.com/veg/phylotree.js
// which is licensed under the MIT License.

// Global variables containing data passed from Drupal.
var treeOptions;
var treeData;
  var tooltip;//@@@

// This function receives the tree settings and tree data from Drupal.
(function ($, drupalSettings) {

  "use strict";

  // Will be dynamically sized.
  var height = 0;
  var tooltip;

  // Store our function as a property of Drupal.behaviors.
  Drupal.behaviors.TripalPhylotree = {
    attach: function (context, settings) {
      var element = document ? 'html' : context;
      once('bind-phyotreevis-once', element).forEach(function initOnce(doc) {
        // Store settings provided from Drupal.
        treeOptions = drupalSettings.treeOptions;
        // Store the data used for the tree.
        treeData = drupalSettings.treeData;
      });
    }
  }

  // Callback for mouseover event on graph node d.
  var phylogeny_node_mouse_over = function(d) {
    var el = $(this);
    var circle = el.find('circle');
    if (!d.children) {
      // Mouseover on leaf node only changes cursor and
      // appearance if the node is clickable.
      if (d.feature_eid || d.organism_eid) {
        el.attr('cursor', 'pointer');
        circle.attr('fill', 'yellow');
        // only leaf nodes have descriptive text
        var txt = el.find('text');
        txt.attr('font-weight', 'bold');
        var svg = document.getElementById('chado-phylogram');
        var parentRect = svg.offsetParent.getBoundingClientRect();;
        var ttx = d3.event.clientX - parentRect.left + 20;
        var tty = d3.event.clientY - parentRect.top - 10;
        var tip = "Click to view this feature";
        if (d.organism_eid) {
          tip = "Click to view this organism";
        }
        tooltip
          .style("opacity", 0.9)
          .style("left", (ttx + 20) + "px")
          .style("top", (tty - 10) + "px")
          .html(tip);
      }
    }
    else {
      // Interior node, a tooltip is shown if there is text
      // associated, but cursor is not changed because interior
      // nodes are not clickable.
      if (d.name) {
        circle.attr('fill', 'yellow');
        var svg = document.getElementById('chado-phylogram');
        var parentRect = svg.offsetParent.getBoundingClientRect();;
        var ttx = d3.event.clientX - parentRect.left + 20;
        var tty = d3.event.clientY - parentRect.top - 10;
        tooltip
          .style("opacity", 0.9)
          .style("left", ttx + "px")
          .style("top", tty + "px")
          .html(d.name);
      }
    }
  };

  // Callback for mouseout event on graph node d.
  var phylogeny_node_mouse_out = function(d) {
    var el = $(this);
    el.attr('cursor', 'default');
    var circle = el.find('circle');
    if (!d.children) {
      // restore the color based on organism id for leaf nodes
      circle.attr('fill', phylogeny_organism_color(d));
      var txt = el.find('text');
      txt.attr('font-weight', 'normal');
    }
    else {
      // restore interior nodes to white, remove tooltip
      circle.attr('fill', 'white');
    }
    tooltip
      .style("opacity", 0.0)
      .style("top", 0 + "px")
      .style("left", 0 + "px")
      .html('');
  };

  // Callback for mousedown/click event on graph node d.
  var phylogeny_node_mouse_down = function(d) {
    var el = $(this);
    var title = (! d.children ) ? d.name : 'interior node ' + d.phylonode_id;

    if (d.children) {
      // interior node
      if (d.phylonode_id) {
      }
      else {
        // this shouldn't happen but ok
      }
    }
    else {
      // leaf node
      if (d.feature_eid) {
        window.open(baseurl + '/bio_data/' + d.feature_eid, '_blank');
        return;
      }
      // If this node is not associated with a feature but it has an
      // organism node then this is a taxonomic node and we want to
      // link it to the organism page.
      if (!d.feature_id && d.organism_nid) {
        window.open(baseurl + '/node/' + d.organism_nid, '_blank');
      }
      if (!d.feature_id && d.organism_eid) {
        window.open(baseurl + '/bio_data/' + d.organism_eid, '_blank');
      }
    }
  };

  // Creates the tree using the d3.phylogram.js library.
  function phylogeny_display_data(treeData) {
    var height = phylogeny_graph_height(treeData);
    d3.phylogram.build('#chado-phylogram', treeData, {
      'width' : treeOptions['phylogram_width'],
      'height' : height,
      'fill' : phylogeny_organism_color,
      'size' : phylogeny_node_size,
      'nodeMouseOver' : phylogeny_node_mouse_over,
      'nodeMouseOut' : phylogeny_node_mouse_out,
      'nodeMouseDown' : phylogeny_node_mouse_down,
      'skipTicks' : treeOptions['skipTicks'],
      'phylogram_scale' : treeOptions['phylogram_scale']
    });

    // Create a tooltip, used for mouseover.
    tooltip = d3.select('#chado-phylogram')
      .append('div')
      .style('opacity', 0)
      .attr('class', 'tooltip')
      .style('border-style', 'solid')
      .style('border-color', '#B4B4B4')
      .style('background-color', '#E4E4E4')
      .style('border-width', '1px')
      .style('border-radius', '5px')
      .style('padding', '5px')
      .style('position', 'absolute')
      .style('display', 'inline-block')
 }

  /* graphHeight() generate graph height based on leaf nodes */
  function phylogeny_graph_height(data) {
    function count_leaf_nodes(node) {
      if (! node.children) {
        return 1;
      }
      var ct = 0;
      node.children.forEach( function(child) {
        ct+= count_leaf_nodes(child);
      });
      return ct;
    }
    var leafNodeCt = count_leaf_nodes(data);
    return 22 * leafNodeCt;
  }
console.log("CP59 done");//@@@
})(jQuery, drupalSettings);





var phylotree_extensions = new Object();

$("#display_tree").on("click", function(e) {
  tree.options({ branches: "straight" }, true);
});

$("#mp_label").on("click", function(e) {
  tree.maxParsimony(true, "Foreground");
});

$("[data-direction]").on("click", function(e) {
  var which_function =
    $(this).data("direction") == "vertical"
      ? tree.display.spacing_x.bind(tree.display)
      : tree.display.spacing_y.bind(tree.display);
  which_function(which_function() + +$(this).data("amount")).update();
});

$(".phylotree-layout-mode").on("click", function(e) {
  if (tree.display.radial() != ($(this).data("mode") == "radial")) {
    $(".phylotree-layout-mode").toggleClass("active");
    tree.display.radial(!tree.display.radial()).update();
  }
});

$("#toggle_animation").on("click", function(e) {
  var current_mode = $(this).hasClass("active");
  $(this).toggleClass("active");
  tree.options({ transitions: !current_mode });
});

$(".phylotree-align-toggler").on("click", function(e) {
  var button_align = $(this).data("align");
  var tree_align = tree.display.options.alignTips;

  if (tree_align != button_align) {
    tree.display.alignTips(button_align == "right");
    $(".phylotree-align-toggler").toggleClass("active");
    tree.display.update();
  }
});

function sort_nodes(asc) {
  tree.resortChildren(function(a, b) {
    return (b.height - a.height || b.value - a.value) * (asc ? 1 : -1);
  });
}

$("#sort_original").on("click", function(e) {
  tree.resortChildren(function(a, b) {
    return a["original_child_order"] - b["original_child_order"];
  });
});

$("#sort_ascending").on("click", function(e) {
  sort_nodes(true);
  tree.display.update();
});

$("#sort_descending").on("click", function(e) {
  sort_nodes(false);
  tree.display.update();
});

$("#and_label").on("click", function(e) {
  tree.display.internalLabel(function(d) {
    return d.reduce(function(prev, curr) {
      return curr[current_selection_name] && prev;
    }, true);
  }, true);
});

$("#or_label").on("click", function(e) {
  tree.display.internalLabel(function(d) {
    return d.reduce(function(prev, curr) {
      return curr[current_selection_name] || prev;
    }, false);
  }, true);
});

$("#filter_add").on("click", function(e) {
  tree.display
    .modifySelection(function(d) {
      return d.tag || d[current_selection_name];
    })
    .modifySelection(
      function(d) {
        return false;
      },
      "tag",
      false,
      false
    );
});

$("#filter_remove").on("click", function(e) {
  tree.display.modifySelection(function(d) {
    return !d.tag;
  });
});

$("#select_all").on("click", function(e) {
  tree.display.modifySelection(function(d) {
    return true;
  });
});

$("#select_all_internal").on("click", function(e) {
  tree.display.modifySelection(function(d) {
    return !tree.isLeafNode(d.target);
  });
});

$("#select_all_leaves").on("click", function(e) {
  tree.display.modifySelection(function(d) {
    return tree.isLeafNode(d.target);
  });
});

$("#select_none").on("click", function(e) {
  tree.display.modifySelection(function(d) {
    return false;
  });
});

$("#clear_internal").on("click", function(e) {
  tree.display.modifySelection(function(d) {
    return tree.isLeafNode(d.target)
      ? d.target[current_selection_name]
      : false;
  });
});

$("#clear_leaves").on("click", function(e) {
  tree.display.modifySelection(function(d) {
    return !tree.isLeafNode(d.target)
      ? d.target[current_selection_name]
      : false;
  });
});

$("#display_dengrogram").on("click", function(e) {
  tree.display.options({ branches: "step" }, true);
});

$("#branch_filter").on("input propertychange", function(e) {
  var filter_value = $(this).val();

  var rx = new RegExp(filter_value, "i");

  tree.display.modifySelection(n => {
    if (!n.target.data.name) {
      return false;
    }
    m = n.target.data.name.search(rx);
    return filter_value.length && m != -1;
  }, "tag");
});

$("#validate_newick").on("click", function(e) {
  let test_string = $('textarea[id$="nwk_spec"]').val();

  tree = new phylotree.phylotree(test_string);
  global_tree = tree;

  if (!tree["json"]) {
    var warning_div = d3
      .select("#newick_body")
      .selectAll("div  .alert-danger")
      .data([res["error"]]);
    warning_div.enter().append("div");
    warning_div
      .html(function(d) {
        return d;
      })
      .attr("class", "alert-danger");
  } else {
    tree.render({
      container: "#tree_container",
      "draw-size-bubbles": false,
      "node-styler": node_colorizer,
      zoom: false,
      "edge-styler": edge_colorizer
    });

    tree.display.selectionLabel(current_selection_name);

    tree.display.countHandler(count => {
      $("#selected_branch_counter").text(function(d) {
        return count[current_selection_name];
      });
    });

    // Get selection set names from parsed newick
    if (tree.parsed_tags.length) {
      selection_set = tree.parsed_tags;
    }

    update_selection_names();

    $("#newick_modal").modal("hide");
    $(tree.display.container).html(tree.display.show());
  }
});

function default_tree_settings() {
  tree = phylotree();
  tree.branchLength(null);
  tree.branchName(null);
  tree.display.radial(false).separation(function(a, b) {
    return 0;
  });
}

function node_colorizer(element, data) {
  var node_color = "#404040";
  var node_size = treeOptions.root_node_size;
  var tooltiptext = 'Root node';
  var circle = element.selectAll("circle");
  var annotations = null;

  if ("annotation" in data.data) {
    annotations = data.data.annotation;
    tooltiptext = data.data.name;
    if (annotations.cvterm_name == 'phylo_leaf') {
      node_color = "#40A040";
      node_size = treeOptions.leaf_node_size;

      // Use custom organism colors if so configured in the formatter settings.
      var organism_color = treeOptions['org_colors'];
      if (organism_color[annotations.fo_organism_id] !== undefined) {
        node_color = organism_color[annotations.fo_organism_id];
      }
      else if (organism_color[annotations.organism_id] !== undefined) {
        node_color = organism_color[annotations.organism_id];
      }
    }
    else if (annotations.cvterm_name == 'phylo_interior') {
      node_color = "#808080";
      node_size = treeOptions.interior_node_size;
    }
    circle.style("fill", node_color);
    circle.attr("r", node_size);
  }
  else {
    // We left the root node without any annotation.
    circle.style("fill", node_color);
    circle.attr("r", node_size);
  }

  // Install a mouse click event for any nodes linked to other content.
  element.on('click', function(event) {
    if (annotations && annotations.entity_id) {
      window.open(baseurl + '/bio_data/' + annotations.entity_id, '_blank');
    }
    // We don't currently support node links, but we might in the future.
    else if (annotations && annotations.node_id) {
      window.open(baseurl + '/node/' + annotations.node_id, '_blank');
    }
  });

  // Install mouseover events for all nodes with a name.
  if (tooltiptext) {
    if (annotations && annotations.entity_id) {
      tooltiptext += " — Click to view";
    }
    element.on('mouseover', function(event) {
      circle.style('fill', 'yellow');
      var svg = document.getElementById('tree_container');
      var parentRect = svg.offsetParent.getBoundingClientRect();
      var ttx = event.clientX - parentRect.left + 20;
      var tty = event.clientY - parentRect.top - 10;
      tooltip
        .style("opacity", 0.9)
        .style("left", ttx + "px")
        .style("top", tty + "px")
        .html(tooltiptext);
    });
    element.on('mouseout', function(event) {
      circle.style('fill', node_color);
      tooltip
        .style("opacity", 0.0)
        .style("top", 0 + "px")
        .style("left", 0 + "px")
        .html('');
    });
  }


}

var valid_id = new RegExp("^[\\w]+$");

$("#selection_name_box").on("input propertychange", function(e) {
console.log("CPX1");
  var name = $(this).val();

  var accept_name =
    selection_set.indexOf(name) < 0 && valid_id.exec(name);

  d3.select("#save_selection_button").classed(
    "disabled",
    accept_name ? null : true
  );
});

$("#selection_rename").on("click", function(e) {
console.log("CPX2");
  d3.select("#save_selection_button")
    .classed("disabled", true)
    .on("click", function(e) {
      // save selection handler
      var old_selection_name = current_selection_name;
      selection_set[current_selection_id] = current_selection_name = $(
        "#selection_name_box"
      ).val();

      if (old_selection_name != current_selection_name) {
        tree.update_key_name(old_selection_name, current_selection_name);
        update_selection_names(current_selection_id);
      }
      send_click_event_to_menu_objects(
        new CustomEvent(selection_menu_element_action, {
          detail: ["save", this]
        })
      );
    });

  d3.select("#cancel_selection_button")
    .classed("disabled", false)
    .on("click", function(e) {
      // save selection handler
      $("#selection_name_box").val(current_selection_name);
      send_click_event_to_menu_objects(
        new CustomEvent(selection_menu_element_action, {
          detail: ["cancel", this]
        })
      );
    });

  send_click_event_to_menu_objects(
    new CustomEvent(selection_menu_element_action, {
      detail: ["rename", this]
    })
  );
  e.preventDefault();
});

$("#selection_delete").on("click", function(e) {
console.log("CPX3");
  tree.display.updateKeyName(selection_set[current_selection_id], null);
  selection_set.splice(current_selection_id, 1);

  if (current_selection_id > 0) {
    current_selection_id--;
  }
  current_selection_name = selection_set[current_selection_id];
  update_selection_names(current_selection_id);
  $("#selection_name_box").val(current_selection_name);

  send_click_event_to_menu_objects(
    new CustomEvent(selection_menu_element_action, {
      detail: ["save", this]
    })
  );
  e.preventDefault();
});

$("#selection_new").on("click", function(e) {
console.log("CPX4");
  d3.select("#save_selection_button")
    .classed("disabled", true)
    .on("click", function(e) {
      // save selection handler
      current_selection_name = $("#selection_name_box").val();
      current_selection_id = selection_set.length;
      selection_set.push(current_selection_name);
      update_selection_names(current_selection_id);
      send_click_event_to_menu_objects(
        new CustomEvent(selection_menu_element_action, {
          detail: ["save", this]
        })
      );
    });

  d3.select("#cancel_selection_button")
    .classed("disabled", false)
    .on("click", function(e) {
      // save selection handler
      $("#selection_name_box").val(current_selection_name);
      send_click_event_to_menu_objects(
        new CustomEvent(selection_menu_element_action, {
          detail: ["cancel", this]
        })
      );
    });

  send_click_event_to_menu_objects(
    new CustomEvent(selection_menu_element_action, {
      detail: ["new", this]
    })
  );
  e.preventDefault();
});

function send_click_event_to_menu_objects(e) {
console.log("CPX5");
  $(
    "#selection_new, #selection_delete, #selection_rename, #save_selection_name, #selection_name_box, #selection_name_dropdown"
  )
    .get()
    .forEach(function(d) {
      d.dispatchEvent(e);
    });
}

function update_selection_names(id, skip_rebuild) {
console.log("CPX6");
  skip_rebuild = skip_rebuild || false;
  id = id || 0;

  current_selection_name = selection_set[id];
  current_selection_id = id;

  if (!skip_rebuild) {
    d3.selectAll(".selection_set").remove();

    d3.select("#selection_name_dropdown")
      .selectAll(".selection_set")
      .data(selection_set)
      .enter()
      .append("a")
      .attr("class", "selection_set dropdown-item")
      .attr("href", "#")
      .text(function(d) {
        return d;
      })
      .style("color", function(d, i) {
        return color_scheme(i);
      })
      .on("click", function(d, name) {
        // Pass the index of name
        let i = _.indexOf(selection_set, name);
        update_selection_names(i, true);
      });
  }

  d3.select("#selection_name_box")
    .style("color", color_scheme(id))
    .property("value", current_selection_name);

  // Loop through all selection_sets
  _.each(selection_set, function(id) {
    tree.display.selectionLabel(id);
    tree.display.update();
  });

  //console.log('Setting label within the tree display');
  //console.log(id);
  //console.log(selection_set[id]);
  tree.display.selectionLabel(selection_set[id]);
  tree.display.update();
}

var width = 800, //$(container_id).width(),
  height = 800, //$(container_id).height()
  selection_set = ["Foreground"],
  current_selection_name = $("#selection_name_box").val(),
  current_selection_id = 0,
  max_selections = 10;
(color_scheme = d3.scaleOrdinal(d3.schemeCategory10)),
  (selection_menu_element_action = "phylotree_menu_element_action");

var test_string =
  "(((EELA:0.150276,CONGERA:0.213019):0.230956,(EELB:0.263487,CONGERB:0.202633):0.246917):0.094785,((CAVEFISH:0.451027,(GOLDFISH:0.340495,ZEBRAFISH:0.390163):0.220565):0.067778,((((((NSAM:0.008113,NARG:0.014065):0.052991,SPUN:0.061003,(SMIC:0.027806,SDIA:0.015298,SXAN:0.046873):0.046977):0.009822,(NAUR:0.081298,(SSPI:0.023876,STIE:0.013652):0.058179):0.091775):0.073346,(MVIO:0.012271,MBER:0.039798):0.178835):0.147992,((BFNKILLIFISH:0.317455,(ONIL:0.029217,XCAU:0.084388):0.201166):0.055908,THORNYHEAD:0.252481):0.061905):0.157214,LAMPFISH:0.717196,((SCABBARDA:0.189684,SCABBARDB:0.362015):0.282263,((VIPERFISH:0.318217,BLACKDRAGON:0.109912):0.123642,LOOSEJAW:0.397100):0.287152):0.140663):0.206729):0.222485,(COELACANTH:0.558103,((CLAWEDFROG:0.441842,SALAMANDER:0.299607):0.135307,((CHAMELEON:0.771665,((PIGEON:0.150909,CHICKEN:0.172733):0.082163,ZEBRAFINCH:0.099172):0.272338):0.014055,((BOVINE:0.167569,DOLPHIN:0.157450):0.104783,ELEPHANT:0.166557):0.367205):0.050892):0.114731):0.295021)myroot";
var container_id = "#tree_container";

//var tree = phylotree.phylotree(test_string);
//.size([height, width]);

//window.setInterval (function () {});

var example_controls = d3.select("#controls_form").append("form");

//var svg = d3.select(container_id).append("svg")
//    .attr("width", width)
//    .attr("height", height);

function selection_handler_name_box(e) {
console.log("CPX7");
  var name_box = d3.select(this);
  switch (e.detail[0]) {
    case "save":
    case "cancel":
      name_box
        .property("disabled", true)
        .style("color", color_scheme(current_selection_id));

      break;
    case "new":
      name_box
        .property("disabled", false)
        .property("value", "new_selection_name")
        .style("color", color_scheme(selection_set.length));
      break;
    case "rename":
      name_box.property("disabled", false);
      break;
  }
}

function selection_handler_new(e) {
console.log("CPX8");
  var element = d3.select(this);
  $(this).data("tooltip", false);
  switch (e.detail[0]) {
    case "save":
    case "cancel":
      if (selection_set.length == max_selections) {
        element.classed("disabled", true);
        $(this).tooltip({
          title: "Up to " + max_selections + " are allowed",
          placement: "left"
        });
      } else {
        element.classed("disabled", null);
      }
      break;
    default:
      element.classed("disabled", true);
      break;
  }
}

function selection_handler_rename(e) {
console.log("CPX9");
  var element = d3.select(this);
  element.classed(
    "disabled",
    e.detail[0] == "save" || e.detail[0] == "cancel" ? null : true
  );
}

function selection_handler_save_selection_name(e) {
console.log("CPX10");
  var element = d3.select(this);
  element.style(
    "display",
    e.detail[0] == "save" || e.detail[0] == "cancel" ? "none" : null
  );
}

function selection_handler_name_dropdown(e) {
console.log("CPX11");
  var element = d3.select(this).selectAll(".selection_set");
  element.classed(
    "disabled",
    e.detail[0] == "save" || e.detail[0] == "cancel" ? null : true
  );
}

function selection_handler_delete(e) {
console.log("CPX12");
  var element = d3.select(this);
  $(this).tooltip("dispose");
  switch (e.detail[0]) {
    case "save":
    case "cancel":
      if (selection_set.length == 1) {
        element.classed("disabled", true);
        $(this).tooltip({
          title:
            "At least one named selection set <br> is required;<br>it can be empty, however",
          placement: "bottom",
          html: true
        });
      } else {
        element.classed("disabled", null);
      }
      break;
    default:
      element.classed("disabled", true);
      break;
  }
}

var datamonkey_save_image = function(type, container) {
console.log("CPX13");
  var prefix = {
    xmlns: "http://www.w3.org/2000/xmlns/",
    xlink: "http://www.w3.org/1999/xlink",
    svg: "http://www.w3.org/2000/svg"
  };

  function get_styles(doc) {
    function process_stylesheet(ss) {
      try {
        if (ss.cssRules) {
          for (var i = 0; i < ss.cssRules.length; i++) {
            var rule = ss.cssRules[i];
            if (rule.type === 3) {
              // Import Rule
              process_stylesheet(rule.styleSheet);
            } else {
              // hack for illustrator crashing on descendent selectors
              if (rule.selectorText) {
                if (rule.selectorText.indexOf(">") === -1) {
                  styles += "\n" + rule.cssText;
                }
              }
            }
          }
        }
      } catch (e) {
        //console.log("Could not process stylesheet : " + ss); // eslint-disable-line
      }
    }

    var styles = "",
      styleSheets = doc.styleSheets;

    if (styleSheets) {
      for (var i = 0; i < styleSheets.length; i++) {
        process_stylesheet(styleSheets[i]);
      }
    }

    return styles;
  }

  var svg = $(container).find("svg")[0];
  if (!svg) {
    svg = $(container)[0];
  }

  var styles = get_styles(window.document);

  svg.setAttribute("version", "1.1");

  var defsEl = document.createElement("defs");
  svg.insertBefore(defsEl, svg.firstChild);

  var styleEl = document.createElement("style");
  defsEl.appendChild(styleEl);
  styleEl.setAttribute("type", "text/css");

  // removing attributes so they aren't doubled up
  svg.removeAttribute("xmlns");
  svg.removeAttribute("xlink");

  // These are needed for the svg
  if (!svg.hasAttributeNS(prefix.xmlns, "xmlns")) {
    svg.setAttributeNS(prefix.xmlns, "xmlns", prefix.svg);
  }

  if (!svg.hasAttributeNS(prefix.xmlns, "xmlns:xlink")) {
    svg.setAttributeNS(prefix.xmlns, "xmlns:xlink", prefix.xlink);
  }

  var source = new XMLSerializer()
    .serializeToString(svg)
    .replace("</style>", "<![CDATA[" + styles + "]]></style>");
  var doctype =
    '<?xml version="1.0" standalone="no"?><!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">';
  var to_download = [doctype + source];
  var image_string =
    "data:image/svg+xml;base66," + encodeURIComponent(to_download);

  if (navigator.msSaveBlob) {
    // IE10
    download(image_string, "image.svg", "image/svg+xml");
  } else if (type == "png") {
    b64toBlob(
      image_string,
      function(blob) {
        var url = window.URL.createObjectURL(blob);
        var pom = document.createElement("a");
        pom.setAttribute("download", "image.png");
        pom.setAttribute("href", url);
        $("body").append(pom);
        pom.click();
        pom.remove();
      },
      function(error) {
        console.log(error); // eslint-disable-line
      }
    );
  } else {
    var pom = document.createElement("a");
    pom.setAttribute("download", "image.svg");
    pom.setAttribute("href", image_string);
    $("body").append(pom);
    pom.click();
    pom.remove();
  }
};

// Converts the json we passed in as annotations to hash objects.
var parse_annotations = function(tree) {
  tree.traverse_and_compute(function(node) {
    if ("annotation" in node.data) {
      var annotation = '{' + node.data.annotation + '}';
      node.data.annotation = JSON.parse(annotation);
    }
  });
};

$(document).ready(function() {
  // Construct the tree from the newick definition.
  tree = new phylotree.phylotree(treeData);

  // Parse our additional json annotations.
  parse_annotations(tree);

  // Create a tooltip, used for mouseover.
  tooltip = d3.select('#tree_container')
    .append('div')
    .style('opacity', 0)
    .attr('class', 'tooltip')
    .style('border-style', 'solid')
    .style('border-color', '#B4B4B4')
    .style('background-color', '#E4E4E4')
    .style('border-width', '1px')
    .style('border-radius', '5px')
    .style('padding', '5px')
    .style('position', 'absolute')
    .style('display', 'inline-block')

  global_tree = tree;

  // Do not show the scale for trees with distance 0.001 placeholders
  // because phylotree.js rounds scale tics to two decimal places.
  var show_scale = false
  var max_length = Math.max(tree.getBranchLengths());
  if (max_length >= 0.01) {
    show_scale = true;
  }

  tree.render({
    container: "#tree_container",
    // Setting this true draws the leaf node circles.
    "draw-size-bubbles": true,
    // This function is defined above.
    "node-styler": node_colorizer,
    "font-size": 12,
    zoom: false,
    "edge-styler": null,
    "show-scale": show_scale
  });

  $('#tree_container').on('reroot', function (e) {
    update_selection_names();

    tree.display.countHandler(count => {
      $("#selected_branch_counter").text(function(d) {
        return count[current_selection_name];
      });
    });

  });

  tree.display.selectionLabel(current_selection_name);

  tree.display.countHandler(count => {
    $("#selected_branch_counter").text(function(d) {
      return count[current_selection_name];
    });
  });

  // Get selection set names from parsed newick
  if (tree.parsed_tags.length) {
    selection_set = tree.parsed_tags;
  }

  // Until a cleaner solution to supporting both Observable and regular HTML
  $(tree.display.container).append(tree.display.show());

if (0) {
  $("#selection_new")
    .get(0)
    .addEventListener(
      selection_menu_element_action,
      selection_handler_new,
      false
    );
  $("#selection_rename")
    .get(0)
    .addEventListener(
      selection_menu_element_action,
      selection_handler_rename,
      false
    );
  $("#selection_delete")
    .get(0)
    .addEventListener(
      selection_menu_element_action,
      selection_handler_delete,
      false
    );
  $("#selection_delete")
    .get(0)
    .dispatchEvent(
      new CustomEvent(selection_menu_element_action, {
        detail: ["cancel", null]
      })
    );
  $("#selection_name_box")
    .get(0)
    .addEventListener(
      selection_menu_element_action,
      selection_handler_name_box,
      false
    );
  $("#save_selection_name")
    .get(0)
    .addEventListener(
      selection_menu_element_action,
      selection_handler_save_selection_name,
      false
    );
  $("#selection_name_dropdown")
    .get(0)
    .addEventListener(
      selection_menu_element_action,
      selection_handler_name_dropdown,
      false
    );

  update_selection_names();
}

  $("#save_image").on("click", function(e) {
    datamonkey_save_image("svg", "#tree_container");
  });
console.log("CP09 done");//@@@

});
