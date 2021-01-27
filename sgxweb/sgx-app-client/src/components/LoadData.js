import React, { useEffect } from "react";
import { useHistory } from "react-router-dom";
import PropTypes from 'prop-types';
import { makeStyles, useTheme } from '@material-ui/core/styles';
import { API } from "aws-amplify";
import Table from '@material-ui/core/Table';
import TableBody from '@material-ui/core/TableBody';
import TableCell from '@material-ui/core/TableCell';
import TableContainer from '@material-ui/core/TableContainer';
import TableFooter from '@material-ui/core/TableFooter';
import TablePagination from '@material-ui/core/TablePagination';
import TableRow from '@material-ui/core/TableRow';
import TableHead from '@material-ui/core/TableHead';
import Paper from '@material-ui/core/Paper';
import IconButton from '@material-ui/core/IconButton';
import FirstPageIcon from '@material-ui/icons/FirstPage';
import KeyboardArrowLeft from '@material-ui/icons/KeyboardArrowLeft';
import KeyboardArrowRight from '@material-ui/icons/KeyboardArrowRight';
import LastPageIcon from '@material-ui/icons/LastPage';
import CircularProgress from '@material-ui/core/CircularProgress';
import "./LoadData.css";

const useStyles1 = makeStyles((theme) => ({
  root: {
    flexShrink: 0,
    marginLeft: theme.spacing(2.5),
  },
}));

function TablePaginationActions(props) {
  const classes = useStyles1();
  const theme = useTheme();
  const { count, page, rowsPerPage, onChangePage } = props;

  const handleFirstPageButtonClick = (event) => {
    onChangePage(event, 0);
  };

  const handleBackButtonClick = (event) => {
    onChangePage(event, page - 1);
  };

  const handleNextButtonClick = (event) => {
    onChangePage(event, page + 1);
  };

  const handleLastPageButtonClick = (event) => {
    onChangePage(event, Math.max(0, Math.ceil(count / rowsPerPage) - 1));
  };

  return (
    <div className={classes.root}>
      <IconButton
        onClick={handleFirstPageButtonClick}
        disabled={page === 0}
        aria-label="first page"
      >
        {theme.direction === 'rtl' ? <LastPageIcon /> : <FirstPageIcon />}
      </IconButton>
      <IconButton onClick={handleBackButtonClick} disabled={page === 0} aria-label="previous page">
        {theme.direction === 'rtl' ? <KeyboardArrowRight /> : <KeyboardArrowLeft />}
      </IconButton>
      <IconButton
        onClick={handleNextButtonClick}
        disabled={page >= Math.ceil(count / rowsPerPage) - 1}
        aria-label="next page"
      >
        {theme.direction === 'rtl' ? <KeyboardArrowLeft /> : <KeyboardArrowRight />}
      </IconButton>
      <IconButton
        onClick={handleLastPageButtonClick}
        disabled={page >= Math.ceil(count / rowsPerPage) - 1}
        aria-label="last page"
      >
        {theme.direction === 'rtl' ? <FirstPageIcon /> : <LastPageIcon />}
      </IconButton>
    </div>
  );
}

TablePaginationActions.propTypes = {
  count: PropTypes.number.isRequired,
  onChangePage: PropTypes.func.isRequired,
  page: PropTypes.number.isRequired,
  rowsPerPage: PropTypes.number.isRequired,
};

const useStyles2 = makeStyles({
  table: {
    minWidth: 500,
  },
  title: {
    textAlign: 'center',
  },
  pagination_toolbar: {
    paddingRight: '30%',
  },
});

export default function LoadData({data}) {
  const classes = useStyles2();
  const history = useHistory();
  const [page, setPage] = React.useState(0);
  const [rowsPerPage, setRowsPerPage] = React.useState(5);
  const [rows, setRows] = React.useState([])
  const [rowLength, setRowLength] = React.useState(null);
  const [identity, setIdentity] = React.useState('');
  const emptyRows = rowsPerPage - Math.min(rowsPerPage, rowLength - page * rowsPerPage);

  const fetchData = async () => { 
    await data.then(fetched => {
      return fetched;
    }).then(result => setRows(loadData(fetchDataSources(result))));
  };

  useEffect(() => {
    fetchData();
    const timer = setInterval(() => {
      if(rowLength !== rows.length){
        setRowLength(rows.length);
      }
    }, 1000);
    return function cleanup() {
      clearInterval(timer);
    }
  }, []);

  function fetchDataSources(identityId) {
    setIdentity(identityId);
    return API.post("sgx", "/get-data-sources", {
    body: "{\"identity\":\""+identityId+"\"}"
    });
  }

  function createData(name, datetime) {
      return { name, datetime };
  }

  function loadData(d) {
      var rs = [];
      d.then(response => {
        let body = JSON.parse(response.body);
        body.forEach(function(element) {
          let ele = JSON.parse(element);
          let keyArr = ele.Key.split("/");
          let file = keyArr[keyArr.length - 1];
          if(file !== ''){
            rows.push(createData(file, ele.LastModified.split("+")[0]));
          }
        });
      });
      rs.sort((a, b) => (a.datetime > b.datetime ? -1 : 1));
      return rows;
  }

  const handleChangePage = (event, newPage) => {
    setPage(newPage);
  };

  const handleChangeRowsPerPage = (event) => {
    setRowsPerPage(parseInt(event.target.value, 10));
    setPage(0);
  };

  function handleDataSelect(name) {
    let timestamp = new Date().getTime();
    //console.log("Name:", name);
    history.push({pathname:"/mapper", data: {name, identity, timestamp}});
  }

  function renderSpinnerPage() {
    return (
      <Table className={classes.table} aria-label="custom pagination table">
        <TableHead>
          <TableRow>
            <TableCell align="center" colSpan={4} style={{ fontSize: '1.25rem', padding: 10}}>
                START VIZ / LOAD DATA
            </TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
        <TableRow>
            <TableCell align="center" colSpan={4} style={{ fontSize: '1rem', padding: '15%' }}>
              <CircularProgress />
            </TableCell>
          </TableRow>
        </TableBody>
      </Table>
    );
  }

  function renderDefaultPage() {
    return (
      <Table className={classes.table} aria-label="custom pagination table">
        <TableHead>
          <TableRow>
            <TableCell align="center" colSpan={4} style={{ fontSize: '1.25rem', padding: 10}}>
                START VIZ / LOAD DATA
            </TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
        <TableRow>
            <TableCell align="center" colSpan={4} style={{ fontSize: '1rem', padding: '15%' }}>
              Upload data to get started!
            </TableCell>
          </TableRow>
        </TableBody>
      </Table>
    );
  }

  function renderTableBody() {
    return (
      <Table className={classes.table} aria-label="custom pagination table">
        <TableHead>
          <TableRow>
            <TableCell align="center" colSpan={4} style={{ fontSize: '1.25rem', padding: 10}}>
                START VIZ / LOAD DATA
            </TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {(rowsPerPage > 0
            ? rows.slice(page * rowsPerPage, page * rowsPerPage + rowsPerPage)
            : rows
          ).map((row) => (
            <TableRow hover key={row.name} onClick={() => handleDataSelect(row.name)} style={{cursor: 'pointer'}}>
              <TableCell component="th" scope="row" style={{ fontSize: '0.875rem', padding: 12, paddingLeft: 32 }}>
                {row.name}
              </TableCell>
              <TableCell style={{ width: '50%', fontSize: '0.875rem', padding: 12, paddingRight: 32 }} align="right">
                {row.datetime}
              </TableCell>
            </TableRow>
          ))}

          {emptyRows > 0 && (
            <TableRow>
              <TableCell colSpan={6} />
            </TableRow>
          )}
        </TableBody>
        <TableFooter>
          <TableRow>
            <TablePagination
              classes={{toolbar: classes.pagination_toolbar}}
              rowsPerPageOptions={[5]}
              colSpan={2}
              count={rowLength}
              rowsPerPage={rowsPerPage}
              page={page}
              SelectProps={{
                inputProps: { 'aria-label': 'rows per page' },
                native: true,
              }}
              onChangePage={handleChangePage}
              onChangeRowsPerPage={handleChangeRowsPerPage}
              ActionsComponent={TablePaginationActions}
            />
          </TableRow>
        </TableFooter>
      </Table>
    );
  }

  return (
    <TableContainer component={Paper} elevation={3} style={{ height: 330 }}>
      {rowLength === null ? renderSpinnerPage() : rowLength === 0 ? renderDefaultPage() : renderTableBody()}
    </TableContainer>
  );
}
