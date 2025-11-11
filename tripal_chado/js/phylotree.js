// Adapted from https://github.com/veg/phylotree.js
// which is licensed under the MIT License.

// Global variables containing data passed from Drupal.
var treeOptions;
var treeData;
// Holds the mouseover tooltip.
var tooltip;
// "tree_container" is the div id specified in the Drupal field formatter.
var container_id = "tree_container";

// This function receives the tree settings and tree data from Drupal.
(function ($, drupalSettings) {
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
})(jQuery, drupalSettings);

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
    return a["data"]["original_child_order"] - b["data"]["original_child_order"];
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

// This function determines node size and color, and provides
// linkouts to Drupal entities for nodes that have entity_id set.
function node_colorizer(element, data) {
  var node_color = "#404040";
  var node_size = treeOptions.root_node_size;
  var tooltiptext = 'Root node';
  var circle = element.selectAll("circle");
  var annotations = null;

  // Note that the root node will not have any annotation.
  if ("annotation" in data.data) {
    annotations = data.data.annotation;
    tooltiptext = data.data.name;

    // Is there a link to Drupal content?
    var hasLink = false;
    if (annotations.entity_id || annotations.node_id) {
      hasLink = true;
    }

    // Internal nodes that have links will have the same appearance
    // as leaf nodes.
    if (hasLink || annotations.cvterm_name == 'phylo_leaf') {
      node_color = "#40A040";
      node_size = treeOptions.leaf_node_size;

      // Use custom organism colors if so configured in the
      // chado field formatter settings.
      var organism_color = treeOptions['org_colors'];
      if (organism_color[annotations.fo_organism_id] !== undefined) {
        node_color = organism_color[annotations.fo_organism_id];
      }
      else if (organism_color[annotations.organism_id] !== undefined) {
        node_color = organism_color[annotations.organism_id];
      }
    }
    // Style for all interior nodes without links.
    else if (annotations.cvterm_name == 'phylo_interior') {
      node_color = "#808080";
      node_size = treeOptions.interior_node_size;
    }
  }

  // Set the node style.
  circle.style("fill", node_color);
  circle.attr("r", node_size);

  // Install a mouse click event for any nodes linked to other content.
  if (hasLink) {
    element.on('click', function(event) {
      if (annotations && annotations.entity_id) {
        window.open(baseurl + '/bio_data/' + annotations.entity_id, '_blank');
      }
      // We don't currently support node links, but we might in the future.
      else if (annotations && annotations.node_id) {
        window.open(baseurl + '/node/' + annotations.node_id, '_blank');
      }
    });
  }

  // Install mouseover events for all nodes with a name.
  // The node color changes and the node name is displayed.
  if (tooltiptext) {
    if (annotations && annotations.entity_id) {
      tooltiptext += " — Click to view";
    }
    element.on('mouseover', function(event) {
      circle.style('fill', 'orange');
      var svg = document.getElementById(container_id);
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

var datamonkey_save_image = function(type, container) {
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

// Converts the json passed in as annotations into hash objects.
var parse_json_annotations = function(tree) {
  tree.traverse_and_compute(function(node) {
    if ("annotation" in node.data) {
      var annotation = '{' + node.data.annotation + '}';
      // Overwrite original string with parsed object.
      node.data.annotation = JSON.parse(annotation);
    }
  });
};

$(document).ready(function() {

  // Construct the phylotree from the newick definition string in
  // the global variable treeData.
  tree = new phylotree.phylotree(treeData);

  // Parse our additional json annotations.
  parse_json_annotations(tree);

  // Create a tooltip, used for mouseover.
  tooltip = d3.select('#' + container_id)
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

  // Turns off scale bar if requested.
  var show_scale = true;
  if (treeOptions.skipTicks) {
    show_scale = false;
  }

  // Rescale trees with distance 0.001 placeholders because
  // phylotree.js rounds scale tic numbers to two decimal places
  // and this is hardcoded and can't be changed.
  // You should not even display a scale for species trees.
  var max_length = Math.max(...tree.getBranchLengths());
  if (max_length < 0.01) {
    tree.scaleBranchLengths(function(length) {
      return length * 10;
    });
  }

  // Linear vs. radial tree layout.
  var radial = false;
  if (treeOptions.phylogram_layout == "radial") {
    radial = true;
  }

  // Controls font size in the rendered tree.
  // phylotree.js doesn't seem to allow values greater than 12.
  var font_size = 12;
  if (treeOptions.font_size) {
    font_size = treeOptions.font_size;
  }

  // Optional logarithmic transformation, a Tripal 3 legacy,
  // would be controlled by treeOptions.phylogram_scale == "log"
  // but is not currently supported because phylogram.js does
  // not have this option.

  tree.render({
    "container": "#" + container_id,
    // Width does not work here, so we fit to container size.
    "left-right-spacing": "fit-to-size",
    "show-scale": show_scale,
    "is-radial": radial,
    // This must be true in order to have leaf node circles.
    "draw-size-bubbles": true,
    "font-size": font_size,
    // This enables zooming with the mouse wheel, not supported here.
    "zoom": false,
    // We are not supporting any of the branch selection functions.
    "selectable": false,
    // The node_colorizer function is defined in this file above.
    "node-styler": node_colorizer,
    "edge-styler": null
  });

  // Until a cleaner solution to supporting both Observable and regular HTML.
  $(tree.display.container).append(tree.display.show());

// @todo make this work.
//  $("#save_image").on("click", function(e) {
//    datamonkey_save_image("svg", "#" + container_id);
//  });

});
