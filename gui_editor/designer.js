// Component data
let components = [];
let selectedComponent = null;
let componentIdCounter = 0;
let isDragging = false;
let dragOffset = { x: 0, y: 0 };

const canvas = document.getElementById('designCanvas');
const propertiesContent = document.getElementById('propertiesContent');
const codeModal = document.getElementById('codeModal');
const generatedCode = document.getElementById('generatedCode');

// Component templates
const componentTemplates = {
    button: {
        type: 'button',
        class: 'ui-button',
        defaultProps: { text: 'Button', x: 50, y: 50, width: 100, height: 35 },
        render: (props) => props.text
    },
    textbox: {
        type: 'textbox',
        class: 'ui-textbox',
        defaultProps: { placeholder: 'Enter text...', x: 50, y: 100, width: 200, height: 30 },
        render: (props) => props.placeholder
    },
    label: {
        type: 'label',
        class: 'ui-label',
        defaultProps: { text: 'Label', x: 50, y: 150, width: 100, height: 20 },
        render: (props) => props.text
    },
    checkbox: {
        type: 'checkbox',
        class: 'ui-checkbox',
        defaultProps: { text: 'Check me', x: 50, y: 200, width: 120, height: 25 },
        render: (props) => props.text
    },
    dropdown: {
        type: 'dropdown',
        class: 'ui-dropdown',
        defaultProps: { text: 'Select...', x: 50, y: 250, width: 150, height: 30 },
        render: (props) => props.text
    },
    panel: {
        type: 'panel',
        class: 'ui-panel',
        defaultProps: { x: 50, y: 300, width: 200, height: 150 },
        render: () => ''
    }
};

// Drag from toolbox
document.querySelectorAll('.tool').forEach(tool => {
    tool.addEventListener('dragstart', (e) => {
        e.dataTransfer.setData('componentType', tool.dataset.type);
    });
});

// Canvas drop zone
canvas.addEventListener('dragover', (e) => {
    e.preventDefault();
});

canvas.addEventListener('drop', (e) => {
    e.preventDefault();
    const componentType = e.dataTransfer.getData('componentType');
    
    if (componentType && componentType !== 'window' && componentTemplates[componentType]) {
        const rect = canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;
        
        addComponent(componentType, x, y);
    }
});

// Add component
function addComponent(type, x, y) {
    const template = componentTemplates[type];
    if (!template) return;
    
    const component = {
        id: ++componentIdCounter,
        type: type,
        props: { ...template.defaultProps, x, y }
    };
    
    components.push(component);
    renderComponent(component);
    selectComponent(component);
}

// Render component on canvas
function renderComponent(component) {
    const template = componentTemplates[component.type];
    const div = document.createElement('div');
    
    div.className = `ui-component ${template.class}`;
    div.dataset.id = component.id;
    div.style.left = component.props.x + 'px';
    div.style.top = component.props.y + 'px';
    div.style.width = component.props.width + 'px';
    div.style.height = component.props.height + 'px';
    div.textContent = template.render(component.props);
    
    // Add resize handles
    ['nw', 'ne', 'sw', 'se'].forEach(pos => {
        const handle = document.createElement('div');
        handle.className = `resize-handle ${pos}`;
        handle.dataset.position = pos;
        div.appendChild(handle);
    });
    
    // Click to select
    div.addEventListener('mousedown', (e) => {
        if (e.target.classList.contains('resize-handle')) {
            startResize(e, component);
        } else {
            startDrag(e, component);
        }
    });
    
    canvas.appendChild(div);
}

// Drag component
function startDrag(e, component) {
    e.stopPropagation();
    e.preventDefault();
    selectComponent(component);
    
    isDragging = true;
    const rect = canvas.getBoundingClientRect();
    dragOffset.x = e.clientX - rect.left - component.props.x;
    dragOffset.y = e.clientY - rect.top - component.props.y;
    
    document.addEventListener('mousemove', onDrag);
    document.addEventListener('mouseup', stopDrag);
}

function onDrag(e) {
    if (!isDragging || !selectedComponent) return;
    
    const rect = canvas.getBoundingClientRect();
    const newX = e.clientX - rect.left - dragOffset.x;
    const newY = e.clientY - rect.top - dragOffset.y;
    
    selectedComponent.props.x = Math.max(0, Math.min(rect.width - selectedComponent.props.width, newX));
    selectedComponent.props.y = Math.max(0, Math.min(rect.height - selectedComponent.props.height, newY));
    
    updateComponentElement(selectedComponent);
    updateProperties();
}

function stopDrag() {
    isDragging = false;
    document.removeEventListener('mousemove', onDrag);
    document.removeEventListener('mouseup', stopDrag);
}

// Resize component
let isResizing = false;
let resizeHandle = null;

function startResize(e, component) {
    e.stopPropagation();
    isResizing = true;
    resizeHandle = e.target.dataset.position;
    selectComponent(component);
    
    document.addEventListener('mousemove', onResize);
    document.addEventListener('mouseup', stopResize);
}

function onResize(e) {
    if (!isResizing || !selectedComponent) return;
    
    const rect = canvas.getBoundingClientRect();
    const mouseX = e.clientX - rect.left;
    const mouseY = e.clientY - rect.top;
    
    const props = selectedComponent.props;
    
    if (resizeHandle.includes('e')) {
        props.width = Math.max(50, mouseX - props.x);
    }
    if (resizeHandle.includes('s')) {
        props.height = Math.max(20, mouseY - props.y);
    }
    
    updateComponentElement(selectedComponent);
    updateProperties();
}

function stopResize() {
    isResizing = false;
    resizeHandle = null;
    document.removeEventListener('mousemove', onResize);
    document.removeEventListener('mouseup', stopResize);
}

// Update component DOM element
function updateComponentElement(component) {
    const el = canvas.querySelector(`[data-id="${component.id}"]`);
    if (el) {
        el.style.left = component.props.x + 'px';
        el.style.top = component.props.y + 'px';
        el.style.width = component.props.width + 'px';
        el.style.height = component.props.height + 'px';
        
        const template = componentTemplates[component.type];
        if (template) {
            el.textContent = template.render(component.props);
            // Re-add resize handles
            ['nw', 'ne', 'sw', 'se'].forEach(pos => {
                const handle = document.createElement('div');
                handle.className = `resize-handle ${pos}`;
                handle.dataset.position = pos;
                el.appendChild(handle);
            });
        }
    }
}

// Select component
function selectComponent(component) {
    selectedComponent = component;
    
    // Visual selection
    canvas.querySelectorAll('.ui-component').forEach(el => {
        el.classList.remove('selected');
    });
    const el = canvas.querySelector(`[data-id="${component.id}"]`);
    if (el) el.classList.add('selected');
    
    showProperties(component);
}

// Show properties panel
function showProperties(component) {
    const template = componentTemplates[component.type];
    
    let html = `<h4 style="margin-bottom: 15px; color: #e74c3c;">${component.type.toUpperCase()} #${component.id}</h4>`;
    
    // Text property
    if (component.props.text !== undefined) {
        html += `
            <div class="property-group">
                <label class="property-label">Text:</label>
                <input type="text" class="property-input" data-prop="text" value="${component.props.text}">
            </div>
        `;
    }
    
    // Placeholder property
    if (component.props.placeholder !== undefined) {
        html += `
            <div class="property-group">
                <label class="property-label">Placeholder:</label>
                <input type="text" class="property-input" data-prop="placeholder" value="${component.props.placeholder}">
            </div>
        `;
    }
    
    // Position & Size
    html += `
        <div class="property-group">
            <label class="property-label">X Position:</label>
            <input type="number" class="property-input" data-prop="x" value="${component.props.x}">
        </div>
        <div class="property-group">
            <label class="property-label">Y Position:</label>
            <input type="number" class="property-input" data-prop="y" value="${component.props.y}">
        </div>
        <div class="property-group">
            <label class="property-label">Width:</label>
            <input type="number" class="property-input" data-prop="width" value="${component.props.width}">
        </div>
        <div class="property-group">
            <label class="property-label">Height:</label>
            <input type="number" class="property-input" data-prop="height" value="${component.props.height}">
        </div>
    `;
    
    html += `
        <button onclick="deleteComponent(${component.id})" class="btn-secondary" style="width: 100%; margin-top: 15px;">
            🗑 Delete Component
        </button>
    `;
    
    propertiesContent.innerHTML = html;
    
    // Add event listeners
    propertiesContent.querySelectorAll('.property-input').forEach(input => {
        input.addEventListener('input', updateProperty);
    });
}

// Update property
function updateProperty(e) {
    if (!selectedComponent) return;
    
    const prop = e.target.dataset.prop;
    let value = e.target.value;
    
    if (prop === 'x' || prop === 'y' || prop === 'width' || prop === 'height') {
        value = parseInt(value) || 0;
    }
    
    selectedComponent.props[prop] = value;
    updateComponentElement(selectedComponent);
}

// Update properties display
function updateProperties() {
    if (selectedComponent) {
        showProperties(selectedComponent);
    }
}

// Delete component
window.deleteComponent = function(id) {
    components = components.filter(c => c.id !== id);
    const el = canvas.querySelector(`[data-id="${id}"]`);
    if (el) el.remove();
    
    selectedComponent = null;
    propertiesContent.innerHTML = '<p class="no-selection">Select a component to view properties</p>';
};

// Clear all
document.getElementById('clearBtn').addEventListener('click', () => {
    if (confirm('Clear all components?')) {
        components = [];
        canvas.querySelectorAll('.ui-component').forEach(el => el.remove());
        selectedComponent = null;
        propertiesContent.innerHTML = '<p class="no-selection">Select a component to view properties</p>';
    }
});

// Generate code
document.getElementById('generateCodeBtn').addEventListener('click', generateCode);

function generateCode() {
    let code = '# Generated MLP GUI Code\n';
    code += '# lang: en-US\n';
    code += '# syntax: mlp\n\n';
    
    // Add simple print statements (no GUI functions for now)
    code += 'print("=== MLP GUI Designer Output ===")\n';
    code += 'print("Window: My Window (800x600)")\n';
    code += 'print("")\n\n';
    
    code += '# Components:\n';
    
    components.forEach(comp => {
        const p = comp.props;
        switch(comp.type) {
            case 'button':
                code += `print("  - Button: \\"${p.text}\\" at (${p.x}, ${p.y}) size ${p.width}x${p.height}")\n`;
                break;
            case 'textbox':
                code += `print("  - TextBox at (${p.x}, ${p.y}) width ${p.width}")\n`;
                break;
            case 'label':
                code += `print("  - Label: \\"${p.text}\\" at (${p.x}, ${p.y})")\n`;
                break;
            case 'checkbox':
                code += `print("  - CheckBox: \\"${p.text}\\" at (${p.x}, ${p.y})")\n`;
                break;
            case 'dropdown':
                code += `print("  - DropDown at (${p.x}, ${p.y}) width ${p.width}")\n`;
                break;
            case 'panel':
                code += `print("  - Panel at (${p.x}, ${p.y}) size ${p.width}x${p.height}")\n`;
                break;
        }
    });
    
    code += '\nprint("")\n';
    code += 'print("Total components: " + ' + components.length + ')\n';
    code += 'print("GUI Designer: Layout complete!")\n';
    
    generatedCode.textContent = code;
    codeModal.classList.add('active');
}

// Modal controls
document.querySelector('.close-btn').addEventListener('click', () => {
    codeModal.classList.remove('active');
});

document.getElementById('copyCodeBtn').addEventListener('click', () => {
    navigator.clipboard.writeText(generatedCode.textContent);
    alert('Code copied to clipboard!');
});

document.getElementById('exportCodeBtn').addEventListener('click', () => {
    const blob = new Blob([generatedCode.textContent], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'gui_app.mlp';
    a.click();
    URL.revokeObjectURL(url);
});

// Run code
document.getElementById('runBtn').addEventListener('click', async () => {
    generateCode();
    
    const code = generatedCode.textContent;
    
    try {
        const response = await fetch('/api/run', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ code })
        });
        
        const result = await response.json();
        
        if (result.success) {
            alert('✅ Program compiled and running!\n\nOutput:\n' + result.output);
        } else {
            alert('❌ Error:\n\n' + result.error);
        }
    } catch (error) {
        alert('❌ Failed to run: ' + error.message);
    }
});
