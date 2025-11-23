import React, { useCallback } from 'react';
import ReactFlow, { 
  Background, 
  Controls, 
  MiniMap,
  addEdge
} from 'reactflow';
import 'reactflow/dist/style.css';

import ButtonNode from './nodes/ButtonNode';
import LabelNode from './nodes/LabelNode';

const nodeTypes = {
  buttonNode: ButtonNode,
  labelNode: LabelNode,
};

function FlowCanvas({ nodes, edges, setNodes, setEdges, onNodeSelect, onInit, onDragOver, onDrop }) {
  const onNodesChange = useCallback(
    (changes) => {
      setNodes((nds) => {
        const updated = [...nds];
        changes.forEach(change => {
          if (change.type === 'position') {
            const node = updated.find(n => n.id === change.id);
            if (node && change.position) {
              node.position = change.position;
            }
          } else if (change.type === 'select') {
            const node = updated.find(n => n.id === change.id);
            if (node) {
              node.selected = change.selected;
              if (change.selected) {
                onNodeSelect(node);
              }
            }
          }
        });
        return updated;
      });
    },
    [setNodes, onNodeSelect]
  );

  const onEdgesChange = useCallback(
    (changes) => setEdges((eds) => eds),
    [setEdges]
  );

  const onConnect = useCallback(
    (params) => setEdges((eds) => addEdge(params, eds)),
    [setEdges]
  );

  const onNodeClick = useCallback(
    (event, node) => {
      onNodeSelect(node);
    },
    [onNodeSelect]
  );

  return (
    <div style={{ width: '100%', height: '100%' }}>
      <ReactFlow
        nodes={nodes}
        edges={edges}
        onNodesChange={onNodesChange}
        onEdgesChange={onEdgesChange}
        onConnect={onConnect}
        onNodeClick={onNodeClick}
        onInit={onInit}
        onDrop={onDrop}
        onDragOver={onDragOver}
        nodeTypes={nodeTypes}
        fitView
        snapToGrid
        snapGrid={[15, 15]}
      >
        <Background color="#333" gap={15} />
        <Controls />
        <MiniMap 
          nodeColor="#666"
          maskColor="rgba(0, 0, 0, 0.6)"
        />
      </ReactFlow>
    </div>
  );
}

export default FlowCanvas;
