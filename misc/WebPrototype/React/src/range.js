import React, { Component } from 'react';
import TextField from 'material-ui/TextField';
import './range.css';

class Range extends React.Component {

    constructor(props) {
        super(props);

        //  this.state.products = [];
        this.state = {};
        this.state.filterText = "";
        this.state.products = [
            {
                id: 6,
                category: 'Electronics',
                price: '199.99',
                qty: 23,
                name: 'nexus 7'
            }
        ];

    }

    handleUserInput(filterText) {
        this.setState({filterText: filterText});
    };

    handleRowDel(product) {
        var index = this.state.products.indexOf(product);
        this.state.products.splice(index, 1);
        this.setState(this.state.products);
    };

    handleAddEvent(evt) {
        var id = (+ new Date() + Math.floor(Math.random() * 999999)).toString(36);
        var product = {
            id: id,
            name: "",
            price: "",
            category: "",
            qty: 0
        }
        this.state.products.push(product);
        this.setState(this.state.products);

    }

    handleRangeTable(evt) {
        var item = {
            id: evt.target.id,
            name: evt.target.name,
            value: evt.target.value
        };
        var products = this.state.products.slice();
        var newProducts = products.map(function(product) {
            for (var key in product) {
                if (key == item.name && product.id == item.id) {
                    product[key] = item.value;
                }
            }
            return product;
        });
        this.setState({products:newProducts});
    };

    render() {

        return (
            <div>
                <SearchRange filterText={this.state.filterText} onUserInput={this.handleUserInput.bind(this)}/>
                <RangeTable onRangeTableUpdate={this.handleRangeTable.bind(this)} onRowAdd={this.handleAddEvent.bind(this)} onRowDel={this.handleRowDel.bind(this)} products={this.state.products} filterText={this.state.filterText}/>
            </div>
        );
    }

}


class SearchRange extends React.Component {
    handleChange() {
        this.props.onUserInput(this.refs.filterTextInput.value);
    }

    render() {
        return (
            <div>
                <input type="text" placeholder="Search..." value={this.props.filterText} ref="filterTextInput" onChange={this.handleChange.bind(this)}/>
            </div>
        );
    }
}

class RangeTable extends React.Component {

    render() {
        var onRangeTableUpdate = this.props.onRangeTableUpdate;
        var rowDel = this.props.onRowDel;
        var filterText = this.props.filterText;
        var product = this.props.products.map(function(product) {
            if (product.name.indexOf(filterText) === -1) {
                return;
            }
            return (<RangeRow onRangeTableUpdate={onRangeTableUpdate} product={product} onDelEvent={rowDel.bind(this)} key={product.id}/>)
        });
        return (
            <div>
                <button type="button" onClick={this.props.onRowAdd} className="btn btn-success pull-right">Add</button>
                {product}
            </div>
        );
    }
}

class RangeRow extends React.Component {
    onDelEvent() {
        this.props.onDelEvent(this.props.product);

    }
    render() {

        return (

            <div>
                <input type="button" onClick={this.onDelEvent.bind(this)} value="X" className="del-btn"/>

                <MinInput onRangeTableUpdate={this.props.onRangeTableUpdate} cellData={{
                    type: "name",
                    value: this.props.product.name,
                    id: this.props.product.id
                }}/>
                <MinInput onRangeTableUpdate={this.props.onRangeTableUpdate} cellData={{
                    type: "price",
                    value: this.props.product.price,
                    id: this.props.product.id
                }}/>
                <MaxInput onRangeTableUpdate={this.props.onRangeTableUpdate} cellData={{
                    type: "qty",
                    value: this.props.product.qty,
                    id: this.props.product.id
                }}/>
                <MaxInput onRangeTableUpdate={this.props.onRangeTableUpdate} cellData={{
                    type: "category",
                    value: this.props.product.category,
                    id: this.props.product.id
                }}/>
                </div>
        );
    }
}

class MinInput extends React.Component {
    render() {
        return (
            <TextField type='text' name={this.props.cellData.type} id={this.props.cellData.id.toString()} value={this.props.cellData.value} onChange={this.props.onRangeTableUpdate}
                hintText="Min"
            />
        );
    }
}

class MaxInput extends React.Component {
    render() {
        return (
            <TextField type='text' name={this.props.cellData.type} id={this.props.cellData.id.toString()} value={this.props.cellData.value} onChange={this.props.onRangeTableUpdate}
                hintText="Max"
            />
        );
    }
}

export default Range;