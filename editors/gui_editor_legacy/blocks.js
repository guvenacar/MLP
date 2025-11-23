// Block definitions with MLP code templates
const blockTemplates = {
    'for': {
        name: 'For Loop',
        color: 'for-block',
        fields: [
            { name: 'variable', label: 'Variable:', value: 'i' },
            { name: 'start', label: 'Start:', value: '0' },
            { name: 'end', label: 'End:', value: '10' },
            { name: 'step', label: 'Step:', value: '', placeholder: 'optional' }
        ],
        hasBody: true,
        generate: (fields) => {
            const step = fields.step ? ` step ${fields.step}` : '';
            return `for ${fields.variable} = ${fields.start} to ${fields.end}${step}`;
        }
    },
    'while': {
        name: 'While Loop',
        color: 'while-block',
        fields: [
            { name: 'condition', label: 'Condition:', value: 'x < 10' }
        ],
        hasBody: true,
        generate: (fields) => `while ${fields.condition}`
    },
    'if': {
        name: 'If Statement',
        color: 'if-block',
        fields: [
            { name: 'condition', label: 'Condition:', value: 'x > 0' }
        ],
        hasBody: true,
        hasElse: true,
        generate: (fields) => `if ${fields.condition} then`
    },
    'variable': {
        name: 'Variable',
        color: 'var-block',
        fields: [
            { name: 'type', label: 'Type:', value: 'SAYISAL', options: ['SAYISAL', 'METIN', 'BOOL'] },
            { name: 'name', label: 'Name:', value: 'x' },
            { name: 'value', label: 'Value:', value: '0' }
        ],
        generate: (fields) => `${fields.type} ${fields.name} = ${fields.value}`
    },
    'assign': {
        name: 'Assignment',
        color: 'var-block',
        fields: [
            { name: 'variable', label: 'Variable:', value: 'x' },
            { name: 'expression', label: 'Expression:', value: 'x + 1' }
        ],
        generate: (fields) => `${fields.variable} = ${fields.expression}`
    },
    'print': {
        name: 'Print',
        color: 'io-block',
        fields: [
            { name: 'value', label: 'Value:', value: 'x' }
        ],
        generate: (fields) => `print(${fields.value})`
    },
    'function': {
        name: 'Function',
        color: 'func-block',
        fields: [
            { name: 'name', label: 'Name:', value: 'myFunc' },
            { name: 'params', label: 'Parameters:', value: '', placeholder: 'a, b' }
        ],
        hasBody: true,
        generate: (fields) => {
            const params = fields.params || '';
            return `işleç ${fields.name}(${params})`;
        }
    },
    'return': {
        name: 'Return',
        color: 'func-block',
        fields: [
            { name: 'value', label: 'Value:', value: 'result' }
        ],
        generate: (fields) => `return ${fields.value}`
    },
    // GUI Components
    'window': {
        name: 'Window',
        color: 'gui-block',
        fields: [
            { name: 'title', label: 'Title:', value: 'My Window' },
            { name: 'width', label: 'Width:', value: '800' },
            { name: 'height', label: 'Height:', value: '600' }
        ],
        generate: (fields) => `window = gui_window_create("${fields.title}", ${fields.width}, ${fields.height})`
    },
    'button': {
        name: 'Button',
        color: 'gui-block',
        fields: [
            { name: 'text', label: 'Text:', value: 'Click Me' },
            { name: 'x', label: 'X:', value: '10' },
            { name: 'y', label: 'Y:', value: '10' },
            { name: 'width', label: 'Width:', value: '100' },
            { name: 'height', label: 'Height:', value: '30' }
        ],
        generate: (fields) => `button = gui_button_create(window, "${fields.text}", ${fields.x}, ${fields.y}, ${fields.width}, ${fields.height})`
    },
    'label': {
        name: 'Label',
        color: 'gui-block',
        fields: [
            { name: 'text', label: 'Text:', value: 'Label' },
            { name: 'x', label: 'X:', value: '10' },
            { name: 'y', label: 'Y:', value: '50' }
        ],
        generate: (fields) => `label = gui_label_create(window, "${fields.text}", ${fields.x}, ${fields.y})`
    },
    'textbox': {
        name: 'TextBox',
        color: 'gui-block',
        fields: [
            { name: 'variable', label: 'Variable:', value: 'textbox1' },
            { name: 'x', label: 'X:', value: '10' },
            { name: 'y', label: 'Y:', value: '90' },
            { name: 'width', label: 'Width:', value: '200' }
        ],
        generate: (fields) => `${fields.variable} = gui_textbox_create(window, ${fields.x}, ${fields.y}, ${fields.width})`
    },
    'panel': {
        name: 'Panel',
        color: 'gui-block',
        fields: [
            { name: 'x', label: 'X:', value: '10' },
            { name: 'y', label: 'Y:', value: '10' },
            { name: 'width', label: 'Width:', value: '300' },
            { name: 'height', label: 'Height:', value: '200' }
        ],
        generate: (fields) => `panel = gui_panel_create(window, ${fields.x}, ${fields.y}, ${fields.width}, ${fields.height})`
    },
    'show_window': {
        name: 'Show Window',
        color: 'gui-block',
        fields: [],
        generate: () => `gui_window_show(window)`
    },
    'event_loop': {
        name: 'Event Loop',
        color: 'gui-block',
        fields: [],
        hasBody: true,
        generate: () => `while gui_poll_event()`
    }
};

// Block class to manage individual blocks on canvas
class Block {
    constructor(type, id = null) {
        this.type = type;
        this.id = id || `block_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
        this.template = blockTemplates[type];
        this.fields = {};
        this.children = []; // For blocks inside loops/conditions
        
        // Initialize fields with default values
        this.template.fields.forEach(field => {
            this.fields[field.name] = field.value;
        });
    }

    generateHTML() {
        const template = this.template;
        let html = `
            <div class="canvas-block" data-id="${this.id}" data-type="${this.type}">
                <div class="block-header ${template.color}">
                    <span>${template.name}</span>
                    <button class="delete-btn" onclick="deleteBlock('${this.id}')">✕</button>
                </div>
                <div class="block-body" style="padding: 12px;">
        `;

        // Generate input fields
        template.fields.forEach(field => {
            if (field.options) {
                // Dropdown
                html += `
                    <div style="margin-bottom: 10px;">
                        <label style="display: block; font-size: 11px; font-weight: 600; color: #555; margin-bottom: 4px;">${field.label}</label>
                        <select data-field="${field.name}" onchange="updateBlockField('${this.id}', '${field.name}', this.value)" 
                                style="width: 100%; padding: 6px; border: 2px solid #ddd; border-radius: 4px; font-size: 12px;">
                            ${field.options.map(opt => 
                                `<option value="${opt}" ${opt === this.fields[field.name] ? 'selected' : ''}>${opt}</option>`
                            ).join('')}
                        </select>
                    </div>
                `;
            } else {
                // Text input
                html += `
                    <div style="margin-bottom: 10px;">
                        <label style="display: block; font-size: 11px; font-weight: 600; color: #555; margin-bottom: 4px;">${field.label}</label>
                        <input type="text" 
                               value="${this.fields[field.name]}" 
                               placeholder="${field.placeholder || ''}"
                               data-field="${field.name}"
                               oninput="updateBlockField('${this.id}', '${field.name}', this.value)"
                               style="width: 100%; padding: 6px; border: 2px solid #ddd; border-radius: 4px; font-family: 'Courier New', monospace; font-size: 12px;">
                    </div>
                `;
            }
        });

        html += `</div>`;
        
        // Body for loops/conditions
        if (template.hasBody) {
            html += `
                <div class="block-body" style="background: #f9f9f9; min-height: 40px; border-top: 2px solid #ddd;">
                    <div class="nested-drop-zone" data-parent="${this.id}">
                        <em style="color: #aaa;">Drop blocks inside...</em>
                    </div>
                </div>
            `;
        }

        html += `</div>`;
        return html;
    }

    generateCode(indent = 0) {
        const template = this.template;
        const indentStr = '    '.repeat(indent);
        let code = indentStr + template.generate(this.fields);

        if (template.hasBody) {
            code += '\n';
            this.children.forEach(child => {
                code += child.generateCode(indent + 1) + '\n';
            });
            code += indentStr + 'end';
        }

        return code;
    }
}

// Global state
window.blocks = [];
window.currentDragBlock = null;
