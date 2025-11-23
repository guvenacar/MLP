import React, { useState, useCallback, useRef } from 'react';
import { ReactFlowProvider } from 'reactflow';
import FlowCanvas from './components/FlowCanvas';
import Toolbar from './components/Toolbar';
import PropertiesPanel from './components/PropertiesPanel';
import MenuBar from './components/MenuBar';
import TopToolbar from './components/TopToolbar';
import './styles/main.css';

function App() {
  const [nodes, setNodes] = useState([]);
  const [edges, setEdges] = useState([]);
  const [selectedNode, setSelectedNode] = useState(null);
  const [showToolbox, setShowToolbox] = useState(true);
  const [showProperties, setShowProperties] = useState(true);
  const [projectName, setProjectName] = useState('Yeni Proje');
  const reactFlowWrapper = useRef(null);
  const [reactFlowInstance, setReactFlowInstance] = useState(null);

  const onNodesChange = useCallback((changes) => {
    setNodes((nds) => {
      // Apply changes to nodes
      return nds; // React Flow will handle this
    });
  }, []);

  const onEdgesChange = useCallback((changes) => {
    setEdges((eds) => {
      // Apply changes to edges
      return eds;
    });
  }, []);

  const onNodeSelect = useCallback((node) => {
    setSelectedNode(node);
  }, []);

  const onAddNode = useCallback((nodeType, position) => {
    const newNode = {
      id: `node-${Date.now()}`,
      type: `${nodeType}Node`,
      position: position || { x: 100, y: 100 },
      data: { 
        label: `New ${nodeType}`,
        width: 120,
        height: 40,
        style: {}
      }
    };
    setNodes((nds) => [...nds, newNode]);
  }, []);

  const onDragOver = useCallback((event) => {
    event.preventDefault();
    event.dataTransfer.dropEffect = 'move';
  }, []);

  const onDrop = useCallback(
    (event) => {
      event.preventDefault();

      const type = event.dataTransfer.getData('application/reactflow');

      if (typeof type === 'undefined' || !type) {
        return;
      }

      const position = reactFlowInstance.screenToFlowPosition({
        x: event.clientX,
        y: event.clientY,
      });

      onAddNode(type, position);
    },
    [reactFlowInstance, onAddNode]
  );

  return (
    <ReactFlowProvider>
      <div className="app">
        <div className="top-bar">
          <MenuBar 
            projectName={projectName}
            onToggleToolbox={() => setShowToolbox(!showToolbox)}
            onToggleProperties={() => setShowProperties(!showProperties)}
          />
          <TopToolbar projectName={projectName} />
        </div>
        
        <div className="main-content">
          {showToolbox && (
            <div className="left-panel">
              <div className="panel-header">Widget Araçları</div>
              <Toolbar />
            </div>
          )}
          
          <div className="center-panel" ref={reactFlowWrapper}>
            <FlowCanvas
              nodes={nodes}
              edges={edges}
              setNodes={setNodes}
              setEdges={setEdges}
              onNodeSelect={onNodeSelect}
              onInit={setReactFlowInstance}
              onDragOver={onDragOver}
              onDrop={onDrop}
            />
          </div>
          
          {showProperties && (
            <div className="right-panel">
              <div className="panel-header">Özellikler</div>
              <PropertiesPanel 
                selectedNode={selectedNode}
                onUpdateNode={(nodeId, updates) => {
                  setNodes((nds) =>
                    nds.map((node) =>
                      node.id === nodeId
                        ? { ...node, data: { ...node.data, ...updates } }
                        : node
                    )
                  );
                }}
              />
            </div>
          )}
        </div>
        
        <div className="status-bar">
          <span id="status-text">Hazır</span>
          <span id="widget-count">Widget Sayısı: {nodes.length}</span>
        </div>
      </div>
    </ReactFlowProvider>
  );
}

export default App;
