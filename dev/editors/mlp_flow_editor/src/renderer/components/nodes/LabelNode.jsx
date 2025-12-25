import React, { memo } from 'react';
import { Handle, Position, NodeResizer } from 'reactflow';

const LabelNode = ({ data, selected }) => {
  const width = data.width || 80;
  const height = data.height || 24;
  
  const defaultStyle = {
    backgroundColor: 'transparent',
    color: '#cccccc',
    border: 'none',
    fontSize: '13px',
    fontFamily: '-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif',
    display: 'flex',
    alignItems: 'center',
    justifyContent: 'flex-start',
    userSelect: 'none',
    padding: '2px 4px',
  };

  return (
    <div
      style={{
        ...defaultStyle,
        ...data.style,
        width: `${width}px`,
        height: `${height}px`,
        border: selected ? '1px dashed #007acc' : 'none',
      }}
    >
      <NodeResizer
        color="#007acc"
        isVisible={selected}
        minWidth={40}
        minHeight={20}
      />
      {data.label || 'Label'}
      <Handle type="target" position={Position.Top} style={{ opacity: 0 }} />
      <Handle type="source" position={Position.Bottom} style={{ opacity: 0 }} />
    </div>
  );
};

export default memo(LabelNode);
